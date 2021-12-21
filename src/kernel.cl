#ifndef CL_KERNEL_SOURCE
#define CL_KERNEL_SOURCE(x) x
#endif // CL_KERNEL_SOURCE

CL_KERNEL_SOURCE(

__constant sampler_t sampler_ = CLK_NORMALIZED_COORDS_TRUE | CLK_ADDRESS_CLAMP | CLK_FILTER_LINEAR;

__constant float half_pi = 3.14159265358979323846 * 0.5;

inline float2 ToNormalizedCoordsi(int2 coords, int2 image_size) {
    return (convert_float2(coords) + (float2)0.5) / convert_float2(image_size);
}

inline float2 ToNormalizedCoordsf(float2 coords, int2 image_size) {
    return (coords + (float2)0.5) / convert_float2(image_size);
}

inline bool IsProcessArea(int2 coords, int2 image_size) {
    if (coords.x < 0 || coords.x >= image_size.x)
        return false;
    if (coords.y < 0 || coords.y >= image_size.y)
        return false;
    return true;
}

inline float2 LinearInterpolation2D(float2 a, float2 b, float alpha) {
    return a + (b - a) * alpha;
}

inline float2 CalcSampleCoords(float2 coords_lt, float2 coords_rt,
                               float2 coords_rb, float2 coords_lb,
                               float2 alpha) {
    float2 interpolated_top    = LinearInterpolation2D(coords_lt, coords_rt, alpha.x);
    float2 interpolated_bottom = LinearInterpolation2D(coords_lb, coords_rb, alpha.x);
    return LinearInterpolation2D(interpolated_top, interpolated_bottom, alpha.y);
}

inline float2 CalcBarrelCoords(float2 coords, float2 center_coords, float focal_distance) {
    // Coords relative to the center
    float2 relative_coords = coords - center_coords;
    // Distance from center
    float distance = length(relative_coords);

    return relative_coords / distance * focal_distance *
           tan(clamp(distance / focal_distance, -half_pi, half_pi)) +
           center_coords;
}

inline float2 CalcSpoolCoords(float2 coords, float2 center_coords, float focal_distance) {
        // Coords relative to the center
        float2 relative_coords = coords - center_coords;
        // Distance from center
        float distance = length(relative_coords);
        return relative_coords / distance * focal_distance *
               atan(distance / focal_distance) + center_coords;
}

__kernel void Spool(read_only image2d_t in_image, write_only image2d_t out_image,
                    int2 image_size, float2 center_coords, float focal_distance) {
    int2 thread_id = (int2)(
        get_global_id(0),
        get_global_id(1)
    );
    // Do nothing if coord is out of process area
    if(!IsProcessArea(thread_id, image_size))
        return;

    float2 coords = convert_float2(thread_id);
    if (coords.x != center_coords.x ||
        coords.y != center_coords.y) {
        coords = CalcSpoolCoords(coords, center_coords, focal_distance);
    }
    float4 pixel_data = read_imagef(in_image, sampler_,
                                    ToNormalizedCoordsf(coords, image_size));
    write_imagef(out_image, thread_id, pixel_data);
}

__kernel void Barrel(read_only image2d_t in_image, write_only image2d_t out_image,
                     int2 image_size, float2 center_coords, float focal_distance) {
    int2 thread_id = (int2)(
        get_global_id(0),
        get_global_id(1)
    );
    // Do nothing if coord is out of process area
    if(!IsProcessArea(thread_id, image_size))
        return;

    float2 coords = convert_float2(thread_id);
    if (coords.x != center_coords.x ||
        coords.y != center_coords.y) {
        // Coords relative to the center
        float2 relative_coords = coords - center_coords;
        // Distance from center
        float distance = length(relative_coords);

        coords = CalcBarrelCoords(coords, center_coords, focal_distance);
    }
    float4 pixel_data = read_imagef(in_image, sampler_,
                                    ToNormalizedCoordsf(coords, image_size));
    write_imagef(out_image, thread_id, pixel_data);
}

__kernel void MultiSamplingBarrel(read_only image2d_t in_image, write_only image2d_t out_image,
                                  int2 image_size, float2 center_coords,
                                  float focal_distance, int max_sampling_per_dimension) {
    int2 thread_id = (int2)(
        get_global_id(0),
        get_global_id(1)
    );
    // Do nothing if coord is out of process area
    if(!IsProcessArea(thread_id, image_size))
        return;

    float2 coords = convert_float2(thread_id);
    // Calc corner coords
    float2 coords_lt = CalcBarrelCoords(coords + (float2)(-0.5, -0.5), center_coords,
                                        focal_distance);
    float2 coords_rt = CalcBarrelCoords(coords + (float2)( 0.5, -0.5), center_coords,
                                        focal_distance);
    float2 coords_rb = CalcBarrelCoords(coords + (float2)( 0.5,  0.5), center_coords,
                                        focal_distance);
    float2 coords_lb = CalcBarrelCoords(coords + (float2)(-0.5,  0.5), center_coords,
                                        focal_distance);
    float4 pixel_data = (float4)0;
    int sampled_num = 0;
    for (int y = 0; y < max_sampling_per_dimension; y++) {
        for (int x = 0; x < max_sampling_per_dimension; x++) {
            float2 alpha = (float2)(
                (float)(x) / (max_sampling_per_dimension),
                (float)(y) / (max_sampling_per_dimension)
            );
            coords = CalcSampleCoords(coords_lt, coords_rt, coords_rb, coords_lb, alpha);
            pixel_data += read_imagef(in_image, sampler_,
                                      ToNormalizedCoordsf(coords, image_size));
            sampled_num++;
        }
    }

    write_imagef(out_image, thread_id, pixel_data / sampled_num);
}

__kernel void Premult(read_only image2d_t in_image, write_only image2d_t out_image,
                      int2 image_size) {
    int2 thread_id = (int2)(
        get_global_id(0),
        get_global_id(1)
    );
    // Do nothing if coord is out of process area
    if(!IsProcessArea(thread_id, image_size))
        return;

    float2 coords = convert_float2(thread_id);
    float4 pixel_data = read_imagef(in_image, sampler_,
                                    ToNormalizedCoordsf(coords, image_size));

    pixel_data.xyz *= pixel_data.w;

    write_imagef(out_image, thread_id, pixel_data);
}

__kernel void Unpremult(read_only image2d_t in_image, write_only image2d_t out_image,
                        int2 image_size) {
    int2 thread_id = (int2)(
        get_global_id(0),
        get_global_id(1)
    );
    // Do nothing if coord is out of process area
    if(!IsProcessArea(thread_id, image_size))
        return;

    float2 coords = convert_float2(thread_id);
    float4 pixel_data = read_imagef(in_image, sampler_,
                                    ToNormalizedCoordsf(coords, image_size));

    float alpha = pixel_data.w;
    if (alpha != 0)
        pixel_data.xyz /= pixel_data.w;

    write_imagef(out_image, thread_id, pixel_data);
}
)
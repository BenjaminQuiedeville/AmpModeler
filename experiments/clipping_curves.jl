
sym_tanh(x) = tanh(x)

asym_tanh(x, low_thresh, high_thresh) = (
    x > high_thresh ? tanh(x - high_thresh) + high_thresh 
    : x < low_thresh ? tanh(x - low_thresh) + low_thresh
    : x)

cubic_clip(x) =  x < -1.0 ? -2.0/3.0 : x - 1.0/3.0 * x^3

cubic_clip_approx(x) = x < -1.0 ? -0.6666 : x - 0.3333 * x^3

cubic_clip_norm(x) = 3 * cubic_clip(x/3)

atan_hard_clip(x) = x > 0 ? 2/π * atan(x * π/2) : x < 1.3 ? 1.3 : x

tanh_cubic(x) = x > 0 ? tanh(x) : cubic_clip_norm

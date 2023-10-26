module SmoothParamCurve

using Plots

const samplerate :: Float64 = 48000;

algebraic(x :: Number) = x / (1 + abs(x))

algebraicSqrt(x :: Number) = x/sqrt(1 + x^2)

tanhCurve(x :: Number) = tanh(x)

cosCurve(x :: Number) = -cos(x * π)

inputAdapt(x :: Number) = 2*x-1

norm(x :: Vector) = (x / maximum(x) .+ 1) / 2

stretch(x :: Number, min :: Number, max :: Number)= x * (max - min) + min
stretch(min :: Number, max :: Number) = x -> stretch(x, min, max)

function main()

    paramMin = 10;
    paramMax = 15;

    X = LinRange(0, 1, 500)

    SGN = sign.(X)
    Y1 = algebraic.(2 * inputAdapt.(X)) |> norm
    Y2 = tanhCurve.(3 * inputAdapt.(X)) |> norm
    Y3 = cosCurve.(X) |> norm

    p1 = plot()
    plot!(X, Y1, label = "algebraic")
    plot!(X, Y2, label = "tanh")
    plot!(X, Y3, label = "cos")

#     plot!(X, X, label = "linear", linestyle = :dash)
#     plot!(X, SGN)
end
main()

end # SmoothParamCurve

using GLMakie

lerp(table :: Vector, index :: Integer, interp_coeff :: Number) :: Number = table[index]*(1-interp_coeff) + table[index+1]*interp_coeff

function lagrange_interpolation_3(table :: Vector, index1 :: Integer, interp_coeff :: Number) :: Number

    index2 = index1 + 1
    index3 = index1 + 2
    index4 = index1 + 3

    if index4 > length(table)
        println("index too high")
        return table[index1]
    end
    

    value1 = table[index1]
    value2 = table[index2]
    value3 = table[index3]
    value4 = table[index4]

    d1 = interp_coeff - 1
    d2 = interp_coeff - 2
    d3 = interp_coeff - 3

    one_sixth = 1/6

    c1 = -d1 * d2 * d3 * one_sixth
    c2 = d2 * d3 * 0.5
    c3 = -d1 * d3 * 0.5
    c4 = d1 * d2 * one_sixth

    return value1 * c1 + interp_coeff * (value2 * c2 + value3 * c3 + value4 * c4)
end 

function test_lagrange() :: Nothing

    sin_pos = [0, 0.1, 0.2, 0.3, 0.4, 0.5, 0.6]

    table = sinpi.(sin_pos)

    interpolated_pos = 0.15
    interpolated = lagrange_interpolation_3(table, 2, 0.5)

    fig, ax, plt = scatter(sin_pos, table)
    scatter!(ax, interpolated_pos, interpolated)
    
    display(fig)

    return nothing
end 



scale(x, min, max, new_min, new_max) = (x - min) / (max - min) * (new_max - new_min) + new_min

scaled_cubic_clip(x) = 3 * cubic_clip(1/3 * x)
cubic_clip(x) = x < -1.0 ? -2/3 : x > 1.0 ? 2/3 : x - 1/3 * x^3

asym_curve(x) = x > 0.0 ? tanh(x) : scaled_cubic_clip(x)


function write_table_to_file(table :: Vector, table_min :: Number, table_max :: Number)

    open("Source/waveshape_table.inc", "w") do file

        table_size = length(table)

        write(file, "#define WAVESHAPE_TABLE_SIZE $(table_size)\n")
        write(file, "static constexpr float table_min = $(table_min)f;\n")
        write(file, "static constexpr float table_max = $(table_max)f;\n\n")
        write(file, "static float waveshaping_table[WAVESHAPE_TABLE_SIZE] = {\n")

        for i in 1:table_size-1 
            write(file, "$(table[i])f, ")
        end 

        write(file, "$(table[end])f")
        write(file, "\n};")

    end 

end 

function main() :: Nothing

    curve = asym_curve
    sampling_interval = 0.01

    table_min = -5
    table_max = 7.0

    lookup_table = curve.(table_min:sampling_interval:table_max)
    table_length = length(lookup_table)

    samplerate = 48000.0
    T = LinRange(0, 0.002, floor(Int, 0.1*samplerate))
    input_signal = 2 * sinpi.(2 * 1000*T)

    output_signal_linear = zeros(size(input_signal))
    output_signal_lagrange = zeros(size(input_signal))

    for i in 1:length(input_signal)

        sample = input_signal[i]

        normalized_position = scale(sample, table_min, table_max, 0, 1)

        table_index = floor(Int, normalized_position * table_length)
        interp_coeff = normalized_position * table_length - table_index

        if table_index >= table_length-1
            println("high clip of the table")
            output_signal_linear[i] = lookup_table[end]
            output_signal_lagrange[i] = lookup_table[end]
            continue
        end 
        if table_index < 1
            println("low clip of the table")
            output_signal_linear[i] = lookup_table[begin]
            output_signal_lagrange[i] = lookup_table[begin]
            continue
        end 

        interpolated_sample_linear = lerp(lookup_table, table_index, interp_coeff)
        interpolated_sample_lagrange = lagrange_interpolation_3(lookup_table, table_index, interp_coeff)

        output_signal_linear[i] = interpolated_sample_linear
        output_signal_lagrange[i] = interpolated_sample_lagrange

    end

    print("table size $(length(lookup_table)), $(length(lookup_table) * 8) bytes as double, $(length(lookup_table) * 4) as float")

    fig = Figure(size = (1920, 1080))
    ax = Axis(fig[1, 1])

    lines!(ax, output_signal_linear, label = "linear interp")
    lines!(ax, output_signal_lagrange, label = "lagrange interp")
    lines!(ax, input_signal, label = "input signal")


    axislegend(ax)

    ax2 = Axis(fig[1, 2])
    lines!(ax2, lookup_table)

    ax3 = Axis(fig[1, 3])
    lines!(ax3, output_signal_lagrange - output_signal_linear, label= "error")
    
    # display(fig)

    write_table_to_file(lookup_table, table_min, table_max)

    return nothing 
end 
main()
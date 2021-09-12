import numpy
import scipy.special

#wih = numpy.random.normal(0.0, pow(3, -0.5), (3, 3))
    # Слой входящий скрытый
wih = numpy.array([[-9.2554719e-10, 0.091299318, -0.40468094],
                   [-0.24745001, 0.71518809, -0.16050813],
                   [-0.3743532, 0.1761861, 0.29734382]])
print(wih)

inputs = numpy.array([2.3, 4.5, 6.7], ndmin=2).T

print(inputs)

hidden_inputs = numpy.dot(wih, inputs)
print(hidden_inputs)

activation_func = lambda x: scipy.special.expit(x)
hidden_outputs = activation_func(hidden_inputs)
print(hidden_outputs)

# Слой скрытый исходящий

who = numpy.array([[-0.66109127, 0.44447052, -0.026807072],
                   [0.7539652, -1.131424, -1.0965117],
                   [-0.31017464, -0.43202519, 1.0949408]])
print(who)

final_inputs = numpy.dot(who, hidden_outputs)
print(final_inputs)

final_outputs = activation_func(final_inputs)
print(final_outputs)
# Обратное распространение ошибки
target = numpy.array([1, 2, 3], ndmin=2).T
print(target)

output_error = target - final_outputs
print(output_error)

hidden_error = numpy.dot(who.T, output_error)
print(hidden_error)

#tmp = output_error * final_outputs * (1.0 - final_outputs)
#print(tmp)
#print(numpy.transpose(hidden_outputs))
#dot = 0.1 * numpy.dot(tmp, numpy.transpose(hidden_outputs))
#print(dot)

who += 0.1 * numpy.dot((output_error * final_outputs * (1.0 - final_outputs)), numpy.transpose(hidden_outputs))
print(who)

wih += 0.1 * numpy.dot((hidden_error * hidden_outputs * (1.0 - hidden_outputs)), numpy.transpose(inputs))
print(wih)

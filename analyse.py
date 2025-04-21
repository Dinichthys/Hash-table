import matplotlib.pyplot as plt
import numpy as np
import math as m

def round_significant(num, digits=1):
    if num == 0:
        return 0
    digit_num = 10 ** (digits - 1 - m.floor(m.log10(abs(num))))
    return round(num * digit_num) / digit_num

iter_num = 1000
version_num = 0
delta = [0, 0, 0, 0]
frequency = 2000000000
avg = [0, 0, 0, 0]
delta_prev = [0, 0, 0, 0]
avg_prev = [0, 0, 0, 0]

file_names = ['data/plot_Base.png',
              'data/plot_StrCmp',
            #   'data/plot_SIMD.png',
              'data/plot_ASM.png',
              'data/plot_InlineASM.png']
# graph_names = ['Базовая версия',
#                'Реализация на массивах',
#                'Реализация с интринсиками размера 256 бит',
#                'Реализация с интринсиками размера 256 бит с полным конвейером']

# plot_stable_names = ['data/plot_Base_.png',
#                      'data/Array_stable.png',
#                      'data/plot_ASM.png',
#                      'data/plot_InlineASM.png']

fig_size = 10,6

version_num = 0
with open('results_base.txt', 'r') as f:
   data = [float(line.strip()) for line in f]

x = np.empty(10, dtype=np.int16)

for i in range (1, 11):
    x [i-1] = i

for i in range (0, 10):
    avg[version_num] += data [i]
avg[version_num] /= 10

for i in range (0, 10):
    delta [version_num] += (avg [version_num] - data [i])**2
delta [version_num] /= 10
delta [version_num] = m.sqrt (delta [version_num])

print ('Величина равна ', avg [version_num], ' а погрешность ',version_num,' версии равна ', int (round_significant (delta [version_num])))

# for i in range (1, 997):
#     if (data[i] > data[i-1]*1.005) & (data[i]> data[i+1]*1.005):
#         data[i] = (data[i-1]+data[i+1])/2

plt.figure(figsize=(fig_size))
plt.plot (x, data)
plt.axis([0, 11, 9.4*10**10, 9.9*10**10])
plt.title("Базовая версия\n Задержка в тактах в зависимости от номера измерения")
plt.xlabel('Номер измерения')
plt.ylabel('Задержка (такты)')
plt.savefig(file_names [version_num], dpi = 300)
plt.clf ()

version_num = 1
with open('results_StrCmp.txt', 'r') as f:
   data = [float(line.strip()) for line in f]

for i in range (0, 10):
    avg[version_num] += data [i]
avg[version_num] /= 10

for i in range (0, 10):
    delta [version_num] += (avg [version_num] - data [i])**2
delta [version_num] /= 10
delta [version_num] = m.sqrt (delta [version_num])

print ('Величина равна ', avg [version_num], ' а погрешность ',version_num,' версии равна ', int (round_significant (delta [version_num])))

# for i in range (1, 997):
#     if (data[i] > data[i-1]*1.005) & (data[i]> data[i+1]*1.005):
#         data[i] = (data[i-1]+data[i+1])/2

plt.figure(figsize=(fig_size))
plt.plot (x, data)
plt.axis([0, 11, 5.2*10**10, 5.6*10**10])
plt.title("Версия с SIMD\n Задержка в тактах в зависимости от номера измерения")
plt.xlabel('Номер измерения')
plt.ylabel('Задержка (такты)')
plt.savefig(file_names [version_num], dpi = 300)
plt.clf ()

version_num = 2
with open('results_ASM.txt', 'r') as f:
   data = [float(line.strip()) for line in f]

for i in range (0, 10):
    avg[version_num] += data [i]
avg[version_num] /= 10

for i in range (0, 10):
    delta [version_num] += (avg [version_num] - data [i])**2
delta [version_num] /= 10
delta [version_num] = m.sqrt (delta [version_num])

print ('Величина равна ', avg [version_num], ' а погрешность ',version_num,' версии равна ', int (round_significant (delta [version_num])))

# for i in range (1, 997):
#     if (data[i] > data[i-1]*1.005) & (data[i]> data[i+1]*1.005):
#         data[i] = (data[i-1]+data[i+1])/2

plt.figure(figsize=(fig_size))
plt.plot (x, data)
plt.axis([0, 11, 3.1*10**10, 3.6*10**10])
plt.title("Версия с переписанной на ассемблер функции\n Задержка в тактах в зависимости от номера измерения")
plt.xlabel('Номер измерения')
plt.ylabel('Задержка (такты)')
plt.savefig(file_names [version_num], dpi = 300)
plt.clf ()

version_num = 3
with open('results_InlineASM.txt', 'r') as f:
   data = [float(line.strip()) for line in f]

for i in range (0, 10):
    avg[version_num] += data [i]
avg[version_num] /= 10

for i in range (0, 10):
    delta [version_num] += (avg [version_num] - data [i])**2
delta [version_num] /= 10
delta [version_num] = m.sqrt (delta [version_num])

print ('Величина равна ', avg [version_num], ' а погрешность ',version_num,' версии равна ', int (round_significant (delta [version_num])))

# for i in range (1, 997):
#     if (data[i] > data[i-1]*1.005) & (data[i]> data[i+1]*1.005):
#         data[i] = (data[i-1]+data[i+1])/2

plt.figure(figsize=(fig_size))
plt.plot (x, data)
plt.axis([0, 11, 3*10**10, 3.5*10**10])
plt.title("Версия с ассемблерной вставкой\n Задержка в тактах в зависимости от номера измерения")
plt.xlabel('Номер измерения')
plt.ylabel('Задержка (такты)')
plt.savefig(file_names [version_num], dpi = 300)
plt.clf ()

for i in range (0, 3):
    avg_prev [3-i] = avg [2-i] / avg [3-i]
    delta_prev [3-i] = avg_prev [3-i] * m.sqrt ((delta [2-i] / avg [2-i])**2 + (delta [3-i] / avg [3-i])**2)
for i in range (0, 4):
    delta [3-i] = avg [0] / avg [3-i] * m.sqrt ((delta [0] / avg [0])**2 + (delta [3-i] / avg [3-i])**2)
    avg [3-i] = avg [0] / avg [3-i]
    print ('Программа ускорилась в ', avg [3-i], ' раз с погрешностью ', float (round_significant (delta [3-i])), ' относительно первой версии и в ',avg_prev[3-i],' относительно предыдущей версии с погрешностью ', delta_prev[3-i])

plt.bar (['Naive', 'SIMD', 'ASM', 'Inline ASM'], avg)
plt.title('Ускорение программы в зависимости\n от способа оптимизации в разах\n относительно наивной версии')
plt.ylabel("Во сколько раз ускорилось")

plt.savefig("data/result_with_StrCmp.png", dpi = 300)

# with open('Output_4.txt', 'r') as f:
#    data = [float(float(line.strip())/1.5) for line in f]
#
# version_num = 0
# plt.hist(data [250 : 751], bins = 40, color = 'blue', edgecolor = 'black', range= (2.70*10**7, 2.74*10**7))
# plt.grid(axis='y', linestyle='--')
# plt.title("Реализация с интринсиками размера 256 бит с полным конвейером\nв компиляторе G++ с флагом -O2")
# plt.xlabel('Задержка (такты)')
# plt.ylabel('Количество повторений')
# plt.savefig(file_names [version_num], dpi = 300)
# plt.clf ()
#
# for i in range (250, 751):
#     avg[version_num] += data [i]
# avg[version_num] /= 500
#
# for i in range (250, 751):
#     delta [version_num] += (avg [version_num] - data [i])**2
# delta [version_num] /= 500
# delta [version_num] = m.sqrt (delta [version_num])
#
# print ('Величина равна ', avg [version_num], ' а погрешность ',version_num,' версии равна ', int (round_significant (delta [version_num])))
#
# with open('Output_4_2.txt', 'r') as f:
#    data = [float(float(line.strip())/1.5) for line in f]
#
# version_num = 1
# plt.hist(data [250 : 751], bins = 40, color = 'blue', edgecolor = 'black', range= (2.70*10**7, 2.74*10**7))
# plt.grid(axis='y', linestyle='--')
# plt.title("Реализация с интринсиками размера 256 бит с полным конвейером\nв компиляторе G++ с флагом -O3")
# plt.xlabel('Задержка (такты)')
# plt.ylabel('Количество повторений')
# plt.savefig(file_names [version_num], dpi = 300)
# plt.clf ()
#
# for i in range (250, 751):
#     avg[version_num] += data [i]
# avg[version_num] /= 500
#
# for i in range (250, 751):
#     delta [version_num] += (avg [version_num] - data [i])**2
# delta [version_num] /= 500
# delta [version_num] = m.sqrt (delta [version_num])
#
# print ('Величина равна ', avg [version_num], ' а погрешность ',version_num,' версии равна ', int (round_significant (delta [version_num])))
#
# with open('Output_4_clang.txt', 'r') as f:
#    data = [float(float(line.strip())/1.5) for line in f]
#
# version_num = 2
# plt.hist(data [250 : 751], bins = 40, color = 'blue', edgecolor = 'black', range= (2.96*10**7, 3.04*10**7))
# plt.grid(axis='y', linestyle='--')
# plt.title("Реализация с интринсиками размера 256 бит с полным конвейером\nв компиляторе clang++ с флагом -O2")
# plt.xlabel('Задержка (такты)')
# plt.ylabel('Количество повторений')
# plt.savefig(file_names [version_num], dpi = 300)
# plt.clf ()
#
# for i in range (250, 751):
#     avg[version_num] += data [i]
# avg[version_num] /= 500
#
# for i in range (250, 751):
#     delta [version_num] += (avg [version_num] - data [i])**2
# delta [version_num] /= 500
# delta [version_num] = m.sqrt (delta [version_num])
#
# print ('Величина равна ', avg [version_num], ' а погрешность ',version_num,' версии равна ', int (round_significant (delta [version_num])))
#
# with open('Output_4_clang_2.txt', 'r') as f:
#    data = [float(float(line.strip())/1.5) for line in f]
#
# version_num = 3
# plt.hist(data [250 : 751], bins = 40, color = 'blue', edgecolor = 'black', range= (2.98*10**7, 3.01*10**7))
# plt.grid(axis='y', linestyle='--')
# plt.title("Реализация с интринсиками размера 256 бит с полным конвейером\nв компиляторе clang++ с флагом -O3")
# plt.xlabel('Задержка (такты)')
# plt.ylabel('Количество повторений')
# plt.savefig(file_names [version_num], dpi = 300)
# plt.clf ()
#
# for i in range (250, 751):
#     avg[version_num] += data [i]
# avg[version_num] /= 500
#
# for i in range (250, 751):
#     delta [version_num] += (avg [version_num] - data [i])**2
# delta [version_num] /= 500
# delta [version_num] = m.sqrt (delta [version_num])
#
# print ('Величина равна ', avg [version_num], ' а погрешность ',version_num,' версии равна ', int (round_significant (delta [version_num])))

# avg = [1, 3.84, 6.08, 14.58]
#
# plt.bar (['Naive', 'Array', 'SIMD 256', 'SIMD 256\nFull Pipeline'], avg)
# plt.ylabel ('Ускорение программы в зависимости\n от способа оптимизации в разах\n относительно наивной версии')
# plt.title("Компилятор G++ с флагом -O2")
#
# plt.savefig("data/g++_O2.png", dpi = 300)
# plt.clf()
#
# avg = [1, 2.71, 6.08, 14.56]
#
# plt.bar (['Naive', 'Array', 'SIMD 256', 'SIMD 256\nFull Pipeline'], avg)
# plt.ylabel ('Ускорение программы в зависимости\n от способа оптимизации в разах\n относительно наивной версии')
# plt.title("Компилятор G++ с флагом -O3")
#
# plt.savefig("data/g++_O3.png", dpi = 300)
# plt.clf()
#
# avg = [1, 3.30, 5.46, 12.9]
#
# plt.bar (['Naive', 'Array', 'SIMD 256', 'SIMD 256\nFull Pipeline'], avg)
# plt.ylabel ('Ускорение программы в зависимости\n от способа оптимизации в разах\n относительно наивной версии')
# plt.title("Компилятор clang++ с флагом -O2")
#
# plt.savefig("data/clang++_O2.png", dpi = 300)
# plt.clf()
#
# avg = [1, 3.30, 5.45, 13.0]
#
# plt.bar (['Naive', 'Array', 'SIMD 256', 'SIMD 256\nFull Pipeline'], avg)
# plt.ylabel ('Ускорение программы в зависимости\n от способа оптимизации в разах\n относительно наивной версии')
# plt.title("Компилятор clang++ с флагом -O3")
#
# plt.savefig("data/clang++_O3.png", dpi = 300)


#
# for version_num in range (0, 4):
#     plt.hist(data [version_num * iter_num + 250 : (version_num + 1) * iter_num - 250], bins = 100, color = 'blue', edgecolor = 'black')
#     plt.grid(axis='y', linestyle='--')
#     plt.title(graph_names [version_num])
#     plt.xlabel('Задержка (такты)')
#     plt.ylabel('Количество повторений')
#     plt.savefig(file_names [version_num], dpi = 300)
#     plt.clf ()
#
#     avg = 0
#     for i in range (version_num * iter_num, (version_num + 1) * iter_num):
#         avg += data [i]
#     avg /= 100
#
#     delta [version_num] = 0
#     for i in range (version_num * iter_num, (version_num + 1) * iter_num):
#         delta [version_num] += (avg - data [i])**2
#     delta [version_num] /= iter_num
#     delta [version_num] = m.sqrt (delta [version_num])
#
#     print ('Погрешность ',version_num,' версии равна ', int (round_significant (delta [version_num])))


# plt.hist(data [100:200], bins = 100, color = 'blue', edgecolor = 'black')
# plt.grid(axis='y', linestyle='--')
# plt.title('Latency distribution (Array)')
# plt.xlabel('Latency')
# plt.ylabel('Quantity')
# plt.savefig('data/plot_Array.png', dpi = 300)
# plt.clf ()
#
# avg = 0
# for i in range (100, 200):
#    avg += data [i]
# avg /= 100
#
# delta = 0
# for i in range (100, 200):
#    delta += (avg - data [i])**2
# delta /= 100
# delta = m.sqrt (delta)
#
# print ('Погрешность версии на массивах равна ', delta)
#
# plt.hist(data [200:300], bins = 100, color = 'blue', edgecolor = 'black')
# plt.grid(axis='y', linestyle='--')
# plt.title('Latency distribution (256 intrinsics)')
# plt.xlabel('Latency')
# plt.ylabel('Quantity')
# plt.savefig('data/plot_256.png', dpi = 300)
# plt.clf ()
#
# avg = 0
# for i in range (200, 300):
#    avg += data [i]
# avg /= 100
#
# delta = 0
# for i in range (200, 300):
#    delta += (avg - data [i])**2
# delta /= 100
# delta = m.sqrt (delta)
#
# print ('Погрешность версии с интринсиками равна ', delta)
#
# plt.hist(data [300:], bins = 100, color = 'blue', edgecolor = 'black')
# plt.grid(axis='y', linestyle='--')
# plt.title('Latency distribution (256 intrinsics in full pipeline)')
# plt.xlabel('Latency')
# plt.ylabel('Quantity')
# plt.savefig('data/plot_256_full_pipeline.png', dpi = 300)
# plt.clf ()
#
# avg = 0
# for i in range (300, 400):
#    avg += data [i]
# avg /= 100
#
# delta = 0
# for i in range (300, 400):
#    delta += (avg - data [i])**2
# delta /= 100
# delta = m.sqrt (delta)
#
# print ('Погрешность версии с интринсиками и загруженным конвейером равна ', delta)

#! encoding: utf-8
import math
import numpy as np


# ---------------------- Здесь будет реализован MCC ----------------------
# Class for minutia. Contains position (x,y) and angle Teta
class M:
    # статические переменные класса
    # инициализация
    def __init__(self):
        self.x = 0
        self.y = 0
        self.teta = 0

    def __init__(self, x, y, teta):
        self.x = x
        self.y = y
        self.teta = teta

Ns = 4 # Ns
Nd = 4 # Nd
# Cuboid discretized into Nc
Nc = Ns * Ns * Nd

# Each cell is small cuboid with deltaS * deltaS base and deltaD height
# deltaS = 2 * R / Ns
# deltaD = 2 * pi / Nd
# i: [1 : Ns]
# j: [1 : Ns]
# k: [1 : Nd]
# Minutia Template


# Угол, соответствующий всем ячейкам цилиндра на уровне k.
def calc_d_fi(k, delta_d):
    return - math.pi + (k - 0.5) * delta_d


# Двумерная точка, соответствующая центру ячейки с индексами i,j
def calc_p_m_i_j(minutia, delda_S):
    VectorXmYm = np.array([minutia.x], [minutia.y])
    RotateMatrix = np.array([np.cos(minutia.teta), np.sin(minutia.teta)], [- np.sin(minutia.teta), np.cos(minutia.teta)])
    #Vector_i_j = np.array([i - (Ns + 1) / 2], [j - (Ns + 1) / 2])

# ---------------------- ---------------------- ----------------------


# ---------------------- Здесь будет реализован LSS ----------------------
# Локальная функция схожести двух минуций (В частности для MCC, схожесть цилиндров Ca и Cb)
# gamma(a,b): A x B -> [0, 1]
# (a + b) - example. This is not similarity between minutia.
def gamma(a, b, similarity):
    return similarity # чего-нить возвращаем (мера схожести зависит от алгоритма)
# Вообще говоря, для MCC gamma имеет реализацию, привязанную к структуре цилиндров.
# Соответственно, здесь не определено, что возвращает функция на самом деле.

# Задаются размеры двух шаблонов минуций A и B.
# Set two minutia templates A and B.
# A = {a_1, a_2, ... , a_Na}
# B = {b_1, b_2, ... , b_Nb}
# So, their sizes are Na and Nb
Na = 10  # Minutia quantity in template A
Nb = 10  # Minutia quantity in template B


# Заполняется gamma-матрица, по которой проходит консолидация локальных сопоставлений в общий результат
# Find local similarity between minutia a from A and b from B: A x B -> [0,1]
def fill_gamma_matrix(gamma_matrix, a, b):
    for i in range(0, Na):
        for j in range(0, Nb):
            gamma_matrix[i][j] = gamma(a[i], b[j])

# Теперь нужно узнать количесто(Np) пар индексов минуций, которые будут взяты из gamma-матрицы
# В статье параметры min_Np, max_Np, mu_p, tau_p просто указаны. Их можно взять за основу

# P - set of selected Np minutia-index pairs
# Np = min_Np + round( Z(min{Na,Nb}, mu_p, tau_p) * (max_Np - min_Np) )
# Z = 1 / (1 + np.exp^(-tau * (v - mu)))
# P = {(Rt, Ct)}, t = 1...Np, Rt = 1...Na, Ct = 1...Nb


# Это сигмоидная функция Z, которая позволяет сглаживать значения.
# Она ограничивает вклад плотных кластеров минуций(обычно шумные регионы) и гарантирует, что
# полученный результат лежит в [0,1]
def calc_z(v, mu, tau):
    return 1 / (1 +  math.e ** (-tau * (v - mu)))

# Значения параметров из статьи
min_np = 4
max_np = 12
mu_p = 20
tau_p = 0.4


# Подсчет количества пар индексов, которые будут взяты из gamma-матрицы
def calc_np(min_np, max_np, na, nb, mu_p, tau_p):
    return (min_np + np.int(np.round(calc_z(min(na, nb), mu_p, tau_p) * (max_np - min_np))))

Np = calc_np(min_np, max_np, Na, Nb, mu_p, tau_p)


# Собственно сам LSS
def similarity(gamma_matrix):
    summary_similarity = 0
    for i in range(0, Na):
        for j in range(0, Nb):
            summary_similarity += gamma_matrix[i][j]
    return summary_similarity/Np
# ---------------------- ---------------------- ----------------------


# Это баловство с python не обращать внимания
#array = np.array([1, 2, 3, 4, 5])
#matrix = np.array([[1, 2, 3], [3, 4, 5], [6, 7, 8]])
#matrix2 = np.array([[1, 2, 3], [3, 4, 5], [6, 7, 8]])
#print(matrix.dot(matrix2))




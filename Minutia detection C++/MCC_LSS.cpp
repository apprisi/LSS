#include <iostream>
#include <opencv2/opencv.hpp>

using namespace cv;
using namespace std;
// Структура, отвечающая некоторой минуции
struct Minutia {
    int x;
    int y;
    int angle; // In range from [0, 2*pi];
    double eps = 0.001;
    Minutia() : x(0), y(0), angle(0) {};
    bool operator != (Minutia const & M) {
        return (x != M.x) || (y != M.y) || (abs(angle - M.angle) < eps);
    }
};

// Евклидово расстояние
double Distance_between_minutiae(Minutia &A, Minutia &B) {
    return sqrt(pow((B.x - A.x), 2) + pow((B.y - A.y), 2));
}

// Евклидово расстояние
double Distance_between_minutiae_and_point(Minutia &A, Point2d &B) {
    return sqrt(pow((B.x - A.x), 2) + pow((B.y - A.y), 2));
}

// Подсчет углов, которые соответствуют всем ячейкам цилиндра на выстотах от 1 до k
void calc_Angles_by_height(vector<double> &d_fi, const double Height_cell) {
    for (int i = 0; i < d_fi.size(); ++i)
        d_fi[i] = - M_PI + ((double)i - 0.5) * Height_cell;
}

// Подсчет двумерных точек, которые соответсвтуют центрам ячеек с индексами i,j, которые спроектированы на основание цилиндра
void calc_two_dim_points(Minutia & M, vector< vector<Point2d> > &Point, const double Base_cell, int Base_cuboid) {
    for (int i = 0; i < Base_cuboid; ++i)
        for (int j = 0; j < Base_cuboid; ++j) {
            double alpha = (double)i - (Base_cuboid + 1.0) / 2.0;
            double beta = (double)j - (Base_cuboid + 1.0) / 2.0;
            Point[i][j].x = M.x + Base_cell * (alpha * cos(M.angle) + beta * sin(M.angle));
            Point[i][j].y = M.y + Base_cell * (-alpha * sin(M.angle) + beta * cos(M.angle));
        }
}

// Для каждой минуции в шаблоне находим соседние минуции.
// Соседнии минуции определяются для каждой из двумерных точек, которые вычисляются для каждой минуции в шаблоне.
// Эти минуции будут вносить вкад от каждой из минуций, для которых будут построены двумерные точки
void calc_neighbours(Minutia & M, vector< vector<Point2d> > &Point, vector < vector< vector<Minutia> > > &Neighbours, vector<Minutia> &T, const double Sigma_spatial) {
    // Смотрим все минуции в шаблоне
    for (int i = 0; i < T.size(); ++i)
        // Идем по всем двумерным точках, построенным для некоторой минуции M
        for (int i = 0; i < Point.size(); ++i)
            for (int j = 0; j < Point.size(); ++j) {
                // Если M не совпадает с минуцией в шаблоне и расстояние от двумерной точки до минуции в шаблоне согласуется с условием,
                // то в для точки P[i][j] минуция T[i] из шаблона T является соседней. Добавим её в вектор соседей.
                    if ((T[i] != M) && (Distance_between_minutiae_and_point(T[i], Point[i][j]) <= 3 * Sigma_spatial))
                        Neighbours[i][j].push_back(T[i]);
            }
}

// Вычисляем пространственный вклад минуции A, отдаваемый в ячейку C_m(i,j,k)
double calc_spatial_gaussian(Minutia &A, Point2d &B, const double Sigma_spatial) {
    // Вычисляем расстояние между координатами минуции А и координатами двумерной точки.
    const double t = Distance_between_minutiae_and_point(A, B);
    // Вычисляем функцию Гаусса с 0 средним и некоторой величиной пространственного отклонения.
    return 1.0 / (Sigma_spatial * sqrt(2.0 * M_PI)) * exp( -pow(t, 2.0) / (2.0 * pow(Sigma_spatial, 2.0)));
}

// Вычисляем разницу между двумя углами
double calc_diff_between_angles(double theta_1, double theta_2) {
    if ((- M_PI <= (theta_1 - theta_2)) && ((theta_1 - theta_2) <= M_PI))
        return theta_1 - theta_2;
    if ((theta_1 - theta_2) < - M_PI)
        return 2.0 * M_PI + theta_1 - theta_2;
    if ((theta_1 - theta_2) >=  M_PI)
        return -2.0 * M_PI + theta_1 - theta_2;
}

// Вычисляем вклад направления минуции.
// Участвуют 3 угла: 1 - угол на высоте k, 2 - угол минуции, для которой считается вклад, угол соседней минуции
double calc_directional_gaussian(double theta_1, double theta_2, double theta_3, const double Sigma_directional, const double Height_cell) {
    // Общий знаменатель
    double denominator = Sigma_directional * sqrt(2.0);
    // Считаем разницу углов между минуциями
    double diff_1 = calc_diff_between_angles(theta_2, theta_3);
    // Считаем разницу между углом на высоте k и предыдущей разницей.
    double alpha = calc_diff_between_angles(theta_1, diff_1);
    // Функция Гаусса в пределах [a - Sigma_directional/2, a + Sigma_directional/2]
    // Счиаем верхний и нижний пределы интеграла.
    // Интеграл заменяется на разницу разницу функций ошибок ERF с некоторыми агрументами
    double top = alpha + Height_cell / 2;
    double bottom = alpha - Height_cell / 2;
    // Функция Гаусса
    return 1 / (Sigma_directional * 2) * (erf(top / denominator) - erf(bottom / denominator));
}

// Сигмоилная функиця, которая гарантирует, что значение V будет лежать в [0, 1]
// nu, tau - параметры
double sigmoid_func(double v, double nu, double tau) {
    return 1 / (1 + exp(- tau * (v - nu)));
}

// Строим выпуклую оболочку для минуций в шаблоне без смещения на Omega пикселей.
void calc_convex_hull(vector<Minutia> &Template, vector<Point2f> &hull) {
    vector<Point2f> Points(Template.size(), Point2f(0,0));
    // Преобразуем координаты минуций в формат Float.
    for (int i = 0; i < Template.size(); ++i) {
        Points[i].x = (float)Template[i].x;
        Points[i].y = (float)Template[i].y;
    }
    // Строим выпуклую оболочку для минуций в шаблоне
    convexHull(Points, hull, false); // bool clockwise = false
}

// Считаем числовое значение C_m(i,j,k) - сумма вкладов всех соседей, лежащих в выпуклой оболочке шаблона минуций и находящихся в на расстоянии
// от текущей минуции M не большем, чем заявленный Radius
void calc_sum_contrinutions(vector< vector < vector<double> > > &C_m, Minutia & M, vector < vector< vector<Minutia> > > &Neighbours,
                              vector< vector<Point2d> > &Point, vector<double> &Angles_by_height, vector<Minutia> & Template, int Radius,
                              const double Sigma_spatial, const double Sigma_directional, int Height_cuboid, const double Height_cell)
{
    // Временная переменная, в нее суммируем вклад соседей
    double sum = 0.0;
    // Параметры для сигмоидной функции
    double nu = 0.01;
    double tau = 400.0;
    // Выпуклая оболочка - вектор точек
    vector<Point2f> ConvexHull(Template.size());
    // Считаем выпуклую оболочку
    calc_convex_hull(Template, ConvexHull);
    // Для точки c индексами [i][j] Смотрим пространственный вклад всех соседей Neighbour[i][j][0...Neighbours[i][j].size()]
    // Рассматриваем этот вклад на каждой высоте от 0...k - 1
    for (int i = 0; i < Neighbours.size(); ++i)
        for (int j = 0; j < Neighbours[i].size(); ++j)
            for (int m = 0; m < Neighbours[i][j].size(); ++m)
                for( int k = 0; k < Height_cuboid; ++k) {
                    // Если центр текущей ячейки внутри выпуклой оболочки
                    // и расстояние от центра этой ячейки до текущей минуции < R, то прибавляем вклад соседей.
                    if ((pointPolygonTest(ConvexHull, Point[i][j], false) == 1) && (Distance_between_minutiae_and_point(M, Point[i][j]) <= (double)Radius))
                    {
                        // Суммируем пространственный вклад * вклад по направлению
                        sum += calc_spatial_gaussian(Neighbours[i][j][m], Point[i][j], Sigma_spatial) *
                               calc_directional_gaussian(Angles_by_height[k], M.angle, Neighbours[i][j][m].angle, Sigma_directional, Height_cell);
                        // Приводим посчитанный вклад к [0, 1]
                        C_m[i][j][k] = sigmoid_func(sum, nu, tau);
                        //cout << "valid" << endl;
                    } else
                        // Если нарушились условия, вклад незначителен.
                        C_m[i][j][k] = - 1; // -1 is invalid
                        //cout << "invalid" << endl;
                }
}

// Линеаризуем цилинидр, соответствующий минуции в вектор
void Linearization(vector< vector < vector<double> > > &C_m, int Base_cuboid, int Height_cuboid, vector<double> &c_m) {
    vector< vector < vector<int> > > Lin(Base_cuboid, vector < vector<int> > (Base_cuboid, vector<int> (Height_cuboid)));
    for (int i = 0; i < C_m.size(); ++i)
        for (int j = 0; j < C_m[i].size(); ++j)
                for (int k = 0; k < C_m[i][j].size(); ++k)
                    Lin[i][j][k] = k * pow(Base_cuboid, 2) + j * Base_cuboid + i;
    for (int i = 0; i < C_m.size(); ++i)
        for (int j = 0; j < C_m[i].size(); ++j)
            for (int k = 0; k < C_m[i][j].size(); ++k)
                c_m[Lin[i][j][k]] = C_m[i][j][k];
}


// Сопоставим два цилиндра, т.е. соответствующие им вектора
void match_minutiae(vector<double> &c_a, vector<double> &c_b, vector<double> &c_a_b, vector<double> &c_b_a) {
    // Проходим по обоим векторам
    for (int i = 0; i < c_a.size(); ++i) {
        // Если элементы сравнимы между собой, то присваиваем соответствующие значеия, иначе 0.
        if ((c_a[i] != -1) && (c_b[i] != -1)) {
            c_a_b[i] = c_a[i];
            c_b_a[i] = c_b[i];
        }
        else {
            c_a_b[i] = 0;
            c_b_a[i] = 0;
        }
    }
}

// Найдем схожесть между двумя цилиндрами
double calc_similarity(Minutia &A, Minutia &B, vector<double> &c_a_b, vector<double> &c_b_a, const double Delta_theta) {
    // Количество сравниваемых элементов при сравнении двух векторов
    int min_me = 0;
    // Вектор, в которй кладем поэлементную разницу двух векторов
    vector<double> diff(c_a_b.size());
    for (int i = 0; i < c_a_b.size(); ++i) {
        // Если элементы сравнимы, увеличим счетчик.
        if ((c_a_b[i] != -1) && (c_b_a[i] != -1))
            ++min_me;
        // Запишем разницу между векторами
        diff[i] = c_a_b[i] - c_b_a[i];
    }
    // Если
    // 1. Разница направлений двух минуций не превышает 90 градусов
    // 2. Количество сравниваемых элементов >= 60 % от всех сравнений
    // 3. Сумма норм сравниваемых векторов != 0, то считаем схожесть. Иначе, 0.
    if ((calc_diff_between_angles(A.angle, B.angle) <= Delta_theta) && (min_me >= 0.6 * c_a_b.size()) && ((norm(c_a_b) + norm(c_b_a)) != 0))
        return 1 - norm(diff) / (norm(c_a_b) + norm(c_b_a));
    else
        return 0;
}


// Тестим выпуклую оболочку
void test_convex_hull() {
    //{ 0, 3 }, { 1, 1 }, { 2, 2 }, { 4, 4 }, { 0, 0 }, { 1, 2 }, { 3, 1 }, { 3, 3 }
    vector<Point2f> Points(8);
    Points[0].x = 0;
    Points[0].y = 3;
    Points[1].x = 1;
    Points[1].y = 1;
    Points[2].x = 2;
    Points[2].y = 2;
    Points[3].x = 4;
    Points[3].y = 4;
    Points[4].x = 0;
    Points[4].y = 0;
    Points[5].x = 1;
    Points[5].y = 2;
    Points[6].x = 3;
    Points[6].y = 1;
    Points[7].x = 3;
    Points[7].y = 3;
    vector<Point2f> hull(8);

    convexHull(Points, hull);
    cout << hull << endl;
    cout << pointPolygonTest(hull, Point2d(7, 2), false);
}


// Local Similarity Sort
double LSS(vector<Minutia> &A, vector<Minutia> &B, vector<double> &c_a_b, vector<double> &c_b_a, const double Delta_theta,
           int max_np, int min_np, int A_size, int B_size, double nu_p, double tau_p)
{
    // Матрица схожестей для двух шаблнов
    vector< vector<double> > Gamma(A_size, vector<double> (B_size));
    vector< pair< double, pair<int, int> > > Linearized_gamma(A.size() * B.size(), make_pair(0.0, make_pair(0, 0)));
    for (int i = 0; i < A_size; ++i)
        for (int j = 0; j < B_size; ++j) {
            // Здесь c_a_b и c_b_a будут считаться для каждых минуций
            Gamma[i][j] = calc_similarity(A[i], B[j], c_a_b, c_b_a, Delta_theta);
            Linearized_gamma[j * A.size() + i] = make_pair(Gamma[i][j], make_pair(i, j));
        }

    // Считаем количество лучших пар индексов минуций, которые войдут в результируюющую сумму
    int np = min_np + (int)round(sigmoid_func(min(A_size, B_size), nu_p, tau_p) * (max_np - min_np));
    double sum = 0.0;
    // Считаем глобальную схожесть.
    for (int i = 0; i < A_size; ++i)
        for (int j = 0; j < B_size; ++j)
            for (int t = 0; t < np; ++t) {}
//                sum += Gamma[]
    return sum;
}

int main() {
    // Определяем константы
    const unsigned int Base_cuboid = 16; // сторона основания квадратной ячейки N_S
    const unsigned int Height_cuboid = 6; // высота ячейки N_D
    const unsigned int Number_cell = Base_cuboid * Base_cuboid * Height_cuboid; // N_C
    unsigned int Radius = 70; // радиус цилиндра
    const double Base_cell = 2.0 * Radius / Base_cuboid; // Delta_S
    const double Height_cell = 2.0 * M_PI / Height_cuboid; // Delta_D
    const double Sigma_spatial = 28.0 / 3.0; // Sigma_S
    const double Sigma_direction = 2.0 * M_PI / 9.0; // Sigma_D
    const double Delta_theta = M_PI / 2;
    // Пусть есть какой-то шаблон. Его считаем из файла
    const unsigned int n = 8;
    vector<Minutia> Template(n);

    // ----------------------------------Построение цилиндра для одной минуции----------------------------------
    // Для каждой минуции будем строит цилиндр, который разобьем на ячейки.
    // Каждая ячейка может быть единственным образом идентифицирована тремя индексами (i, j ,k),
    // которые определяют ее позицию в кубе, опианном вокруг цилиндра.
    // Индексы i,j из I_S = {n из N, 1 <= n <= N_S}, k из I_D = {n из N, 1 <= n <= N_D}

    // Пусть Angles_by_height - вектор углов, которые связываются со всеми ячейками в цилиндре на высоте k, k = 0...Height_cuboid
    // Height_cuboid - высота куба, описывающего цилиндр
    vector<double> Angles_by_height(Height_cuboid);
    // Подсчет углов
    calc_Angles_by_height(Angles_by_height, Height_cell);

    // Рассматриваем некоторую минуцию M из шаблона. Строим для нее цилиндр.
    // Сначала найдем все двумерные точки, соответствующие центрам ячеек с индексами (i,j), спроектированных на основание цилиндра
    // the two-dimensional point corresponding to the center of the cells with indices i; j (projected onto the cylinder’s base),
    // expressed in the spatial coordinates of the minutiae
    Minutia M;
    // Вектор точек, соответствующих центрам ячеек, спроектированных на основание цилиндра
    vector< vector<Point2d> > Two_dim_point(Base_cuboid, vector<Point2d> (Base_cuboid));
    // Считаем эти точки
    calc_two_dim_points(M, Two_dim_point, Base_cell, Base_cuboid);

    // Для каждой из посчитанных точек найдем соседей из шаблона
    // Пустые вектора соседей для каждой точки P^m_i_j
    vector < vector< vector<Minutia> > > Neighbours(Base_cuboid, vector< vector<Minutia> > (Base_cuboid));
    // Ищем соседние минуции для каждой двумерной точки в радиусе 3*Sigma_spatial
    calc_neighbours(M, Two_dim_point, Neighbours, Template, Sigma_spatial);

    // Для каждоый ячейки (i,j,k) получаем числовое значение C_m(i,j,k), вычисленное путем
    // суммирования вкладов всех минуций, которые были соседними для каждой двумерной точки.
    // For each cell (i,j,k) a numerical value C(i,j,k) calculated by accumulating contributions
    // from each minutia m_t belonging to the neighborhood N_p_m_i_j of p_m_i_j
    vector< vector < vector<double> > > Contributions_m(Base_cuboid, vector < vector<double> > (Base_cuboid, vector<double> (Height_cuboid)));
    // Считаем вклад, приводим его в шкалу [0,1].
    calc_sum_contrinutions(Contributions_m, M, Neighbours, Two_dim_point, Angles_by_height, Template, Radius, Sigma_spatial, Sigma_direction, Height_cuboid, Height_cell);

    // Формируем вектор, который будет характеризровать минуцию M
    vector<double> c_m(Number_cell, - 2);
    // Получаем этот вектор путем линеаризации цилиндра минуции M
    Linearization(Contributions_m, Base_cuboid, Height_cuboid, c_m);
    //cerr << "time = " << 1. * clock() / CLOCKS_PER_SEC;
    // ------------------------------------------------------------------------------------------------------

    // ----------------------------------Сравнение двух цилиндров----------------------------------
    vector<double> c_a_b(c_m.size()), c_b_a(c_m.size());
    // Сравниваем два вектора для двух минуций
    match_minutiae(c_m, c_m, c_a_b, c_b_a);
    // Считаем схожесть между двумя минуциями
    calc_similarity(M, M, c_a_b, c_b_a, Delta_theta);
    // ------------------------------------------------------------------------------------------------------


    // ----------------------------------Local Similarity Sort----------------------------------
    // Пусть есть какие-то два шаблона.
    vector<Minutia> A(n);
    vector<Minutia> B(n);
    // Пока что в LSS передадим произвольные c_a_b, c_b_a. Вообще, они должны считаться внутри LSS для каждой пары минуций из A и B.
    const int min_np = 4;
    const int max_np = 12;
    const double nu_p = 20;
    const double tau_p = 0.4;

    LSS(A, B, c_a_b, c_b_a, Delta_theta, max_np, min_np, A.size(), B.size(), nu_p, tau_p);
    return 0;
}

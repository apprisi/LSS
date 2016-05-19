#include <iostream>
#include <opencv2/opencv.hpp>
#include <fstream>

using namespace cv;
using namespace std;

Mat img; // исходное изображение
Mat img2; // изображение, которое перерисовывается, когда мы тянем линию
Mat img3; // копия исходного изображения, используется при удалении элементов

Point point_x1_y1; // начало линии
Point point_x2_y2; // конец линии
bool start_drawing = false; // Флаг для отрисовки "движущейся" линии
vector<Point> Circles; // Здесь храним координаты точек
vector<Point> Lines; // Здесь храним координаты концов линий
Point center; // Для сохранения координат точек
vector<bool> element; // Здесь храним тип элемента: 0 - точка, 1 - линия

void draw(int event, int x, int y, int flags, void *) {
    switch (event) {
        case EVENT_LBUTTONDOWN:
            center = Point(x, y);
            circle(img, center, 3, (0, 0, 255), -1);
            imshow("image", img);
            // Сохраняем координаты отрисованной точки и её тип
            Circles.push_back(center);
            element.push_back(0);
            break;
        case EVENT_FLAG_RBUTTON:
            // Рисуем линию и ее начало
            circle(img, Point(x, y), 3, (0, 0, 255), -1);
            point_x1_y1 = Point(x, y);
            // Сохраняем координаты начала линии
            Lines.push_back(point_x1_y1);
            // Меняем флаг, чтобы линия "тянулась"
            start_drawing = true;
            break;
        case EVENT_RBUTTONUP:
            point_x2_y2 = Point(x, y);
            // Рисуем линию и её конец
            line(img, point_x1_y1, point_x2_y2, (0, 0, 255), 2);
            circle(img, Point(x, y), 3, (0, 0, 255), -1);
            // Сохраняем координаты конца линии и её тип
            Lines.push_back(point_x2_y2);
            element.push_back(1);
            // Меняем флаг, чтобы линия больше не "тянулась"
            start_drawing = false;
            break;
        case EVENT_MOUSEMOVE:
            img.copyTo(img2);
            // Если флаг позволяет линии "тянуться", то рисуем ее, пока тянется.
            if (start_drawing)
                line(img2, point_x1_y1, Point(x, y), (0, 0, 255), 2);
            imshow("image", img2);
            break;
        case 120:
            cout << 1;
        default:
            break;
    }
}


int main() {
    img = imread("fingerprint.jpg", CV_LOAD_IMAGE_COLOR);
    if (!img.data) {
        cerr << "Image not found" << endl;
        return -1;
    }
    namedWindow("image");
    // Отлавливаем движение мыши и рисуем
    setMouseCallback("image", draw);
    // Для сохранения кода нажимаемой клавиши
    int pressed_key = 0;
    // Копируем исходное изображение в "запасное",
    // на котором будем рисовать оставшиеся элементы после удаления какого-нибудь элемента
    img.copyTo(img3);
    // Заводим полоску значений для масштабирования
    createTrackbar("Scale", "image", 0, 10);
    double scale = 0;
    // Показываем изображение и работаем с ним
    imshow("image", img);

    // Здесь масштабируем и удаляем элементы
    while(1) {
        // Получаем код нажатой клавиши
        pressed_key = waitKey(0);
        // Если нажали клавишу M, то пытаемся маштабировать
        if (pressed_key == 109) {
            // Если элементов на изображении нет, ругаемся.
            if (element.size() != 0) {
                cerr << "Scaling after matching is impossible!" << endl;
                continue;
            }
            // Получаем значение масштаба с полоски
            scale = getTrackbarPos("Scale", "image");
            // Масштабируем
            resize(img, img, Size(), 1.0 + scale / 10.0, 1.0 + scale / 10.0, INTER_CUBIC);
            // Сохраняем чистое отмасштабированное изображение
            img.copyTo(img3);
            imshow("image", img);
        }
        // Если нажали X, то убираем 1 элемент.
        if (pressed_key == 120) {
            // Если нечего удалять
            if (element.size() == 0) {
                cerr << "There are no any elements on image" << endl;
                continue;
            }
            // Берем исходное изображение и снова рисуем на нем все, кроме последнего элемента
            img3.copyTo(img);
            // Если удаляем точку
            if (element.back() == 0) {
                // Выкидываем координаты последней точки и удаляем пометку о том, что эта точка была на изображении
                if (Circles.size() != 0) {
                    Circles.pop_back();
                    element.pop_back();
                }
                // Отрисовываем все точки кроме последней
                for (size_t i = 0; i < Circles.size(); ++i)
                    circle(img, Circles[i], 3, (0, 0, 255), -1);
                // Отрисовываем все линии
                for (size_t i = 0; i < Lines.size(); i += 2) {
                    line(img, Lines[i], Lines[i + 1], (0, 0, 255), 2);
                    circle(img, Lines[i], 3, (0, 0, 255), -1);
                    circle(img, Lines[i + 1], 3, (0, 0, 255), -1);
                }
            }
            // Если удаляем линию
            if (element.back() == 1) {
                // Выкидываем координаты концов линии и удаляем пометку о том, что эта линия была на изображении
                if (Lines.size() != 0) {
                    Lines.pop_back();
                    Lines.pop_back();
                    element.pop_back();
                }
                // Отрисовываем все линии кроме последней
                for (size_t i = 0; i < Lines.size(); i += 2) {
                    line(img, Lines[i], Lines[i + 1], (0, 0, 255), 2);
                    circle(img, Lines[i], 3, (0, 0, 255), -1);
                    circle(img, Lines[i + 1], 3, (0, 0, 255), -1);
                }
                // Отрисовываем все точки
                for (size_t i = 0; i < Circles.size(); ++i)
                    circle(img, Circles[i], 3, (0, 0, 255), -1);
            }
            imshow("image", img);
        }
        // По нажатию Esc выходим
        if (pressed_key == 27) {
            imwrite("Matching_image.jpg", img);
            ofstream outFile;
            outFile.open("Data.txt");
            outFile << "Minutiae coordinates: " << endl;
            for (size_t i = 0; i < Circles.size(); ++i)
                outFile << Circles[i] << endl;
            outFile << "Lines coordinates: " << endl;
            for (size_t i = 0; i < Lines.size(); i += 2)
                outFile << Lines[i] << " " << Lines[i + 1] << endl;
            break;
        }
    }
    return 0;
}
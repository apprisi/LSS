//
// Created by montura on 20.05.16.
//
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
vector<int> Angles; // Здесь храним направления минуций
vector<string> Text_coord; // Здесь храним отображаемую информацию для минуций
vector<string> Text_angles; // Здесь храним отображаемую информацию для направлений минуций
bool draw_point_flag = false; // Ограничитель для фиксированного порядка отрисовки: минуций, касательная, минуция, касательная и т.д.

int border_size = 200; // Дополнительная полоска справа для отображения доп. инф.
int number = 0; // порядковый номер минуции
string text_num_coord = ""; // текст, куда пишем номер минуции и ее координаты
string text_angle = "   "; // текст, куда пишем направление минуции
int step_w = 0; // шаг для горизонтального отступа в тексте
int step_h = 0; // шаг для вертикального отступа в тексте
int angle = 0; // величина угла/направления минуции

// Печатаем доп. информацию
void print_text(Mat &image, int number)
{
    text_num_coord = to_string(number) + ". ";
    step_h += getTextSize(text_num_coord, FONT_HERSHEY_COMPLEX, 0.3, 1, 0).height + 3;
    putText(img, text_num_coord, Point(center.x + 5, center.y - 5), FONT_HERSHEY_COMPLEX, 0.35, Scalar(255, 0, 0));
    text_num_coord += "(" + to_string(center.x) + ", " + to_string(center.y) + ")" + ", angle = ";
    putText(img, text_num_coord, Point(img.cols - border_size + 5, step_h), FONT_HERSHEY_COMPLEX, 0.33, Scalar(255, 0, 0));
    step_w = getTextSize(text_num_coord, FONT_HERSHEY_COMPLEX, 0.3, 1, 0).width + 3;
    Text_coord.push_back(text_num_coord);
}

void draw(int event, int x, int y, int flags, void *) {
    switch (event) {
        // Нажали леву кнопку
        case EVENT_LBUTTONDOWN:
            // Заставляем искать направление минуции сразу после того, как её отметили
            if (draw_point_flag) {
                cerr << "Find any angle for previous minutiae" << endl;
                break;
            }
            // Поставили точку
            center = Point(x, y);
            // Нарисовали точку
            circle(img, center, 3, (0, 0, 255), -1, 8);
            // Присвоили ей номер
            ++number;
            // Напечатали номер и координаты
            print_text(img, number);
            // Показали, что нарисовали.
            imshow("image", img);
            // Сохраняем координаты отрисованной точки и её тип
            Circles.push_back(center);
            element.push_back(0);
            draw_point_flag = true;
            break;
        case EVENT_FLAG_RBUTTON:
            // Заставляем отмечать следующую минуцию сразу после того, как определили направление для предыдущей минуции.
            if (!draw_point_flag) {
                cerr << "Match any minutia, before finding any angle" << endl;
                break;
            }
            // Рисуем линию и ее начало
            circle(img, Point(x, y), 3, (0, 0, 255), -1, 8);
            point_x1_y1 = Point(x, y);
            // Сохраняем координаты начала линии
            Lines.push_back(point_x1_y1);
            // Меняем флаг, чтобы линия "тянулась"
            start_drawing = true;
            break;
        case EVENT_RBUTTONUP:
            // Заставляем отмечать следующую минуцию сразу после того, как определили направление для предыдущей минуции.
            if (!draw_point_flag) {
                break;
            }
            point_x2_y2 = Point(x, y);
            // Рисуем линию и её конец
            line(img, point_x1_y1, point_x2_y2, (0, 0, 255), 2);
            circle(img2, Point(x, y), 3, (0, 0, 255), -1, 8);
            // Сохраняем координаты конца линии и её тип
            Lines.push_back(point_x2_y2);
            element.push_back(1);
            // Меняем флаг, чтобы линия больше не "тянулась"
            start_drawing = false;
            // То, что динамически нарисовалось, показываем на оригинальном изображении
            img2.copyTo(img);
            // Добаляем к написанным номеру и координатам направление минуции
            Angles.push_back(angle);
            Text_angles.push_back(text_angle);
            draw_point_flag = false;
            break;
        case EVENT_MOUSEMOVE:
            img.copyTo(img2);
            // Если флаг позволяет линии "тянуться", то рисуем ее, пока тянется.
            if (start_drawing) {
                line(img2, point_x1_y1, Point(x, y), (0, 0, 255), 2);
                // находим угол между касательной и горизонтальной линией сетик изображения
                angle = int(atan((double)(Point(x, y).y - point_x1_y1.y) / (Point(x, y).x - point_x1_y1.x)) * 180.0 / M_PI); // dy / dx
                // Считаем угол против часовой стрелки, поэтому он будет отрицательный
                if (angle <= 0)
                    angle = abs(angle);
                else // случай, когда угол тупой
                    angle = 180 - angle;
                // Определяется 0-й угол, разница меджду координатами концов по У не больше 1 (промежуток в 3 пиксела, когда угол = 0)
                if (Point(x, y).x > point_x1_y1.x && abs(Point(x, y).y - point_x1_y1.y) <= 1)
                    angle = 0;
                    // Когда находимся в нижней полуплоскости, прибавляем 180
                else if (Point(x, y).y >= point_x1_y1.y)
                    angle += 180;
                // Печатаем угол
                text_angle = to_string(angle);
                putText(img2, text_angle, Point(img.cols - border_size + 5 + step_w + 5, step_h), FONT_HERSHEY_COMPLEX, 0.33, Scalar(255, 0, 0));
            }
            imshow("image", img2);
            break;
        case 120:
            cout << 1;
        default:
            break;
    }
}

void draw_element(Mat &img, vector<Point> &Circles, vector<Point> &Lines) {
    // Отрисовываем все линии кроме последней
    for (size_t i = 0; i < Lines.size(); i += 2) {
        line(img, Lines[i], Lines[i + 1], (0, 0, 255), 2);
        circle(img, Lines[i], 3, (0, 0, 255), -1);
        circle(img, Lines[i + 1], 3, (0, 0, 255), -1);
    }
    // Отрисовываем все точки
    for (size_t i = 0; i < Circles.size(); ++i) {
        circle(img, Circles[i], 3, (0, 0, 255), -1);
        text_num_coord = to_string(i + 1) + ". ";
        putText(img, text_num_coord, Point(Circles[i].x + 5, Circles[i].y - 5), FONT_HERSHEY_COMPLEX, 0.35, Scalar(255, 0, 0));
    }
    step_h = 10;
    // Отрисовываем номера и координаты минуций
    for (size_t i = 0; i < Text_coord.size(); ++i) {
        putText(img, Text_coord[i], Point(img.cols - border_size + 5, step_h), FONT_HERSHEY_COMPLEX, 0.33, Scalar(255, 0, 0));
        step_h += getTextSize(Text_coord[i], FONT_HERSHEY_COMPLEX, 0.3, 1, 0).height + 3;
    }
    step_h = 10;
    // Отрисовываем номера и направления минуций
    for (size_t i = 0; i < Text_angles.size(); ++i) {
        step_w = getTextSize(Text_coord[i], FONT_HERSHEY_COMPLEX, 0.3, 1, 0).width + 8;
        putText(img, Text_angles[i], Point(img.cols - border_size + 5 + step_w, step_h), FONT_HERSHEY_COMPLEX, 0.33, Scalar(255, 0, 0));
        step_h += getTextSize(Text_angles[i], FONT_HERSHEY_COMPLEX, 0.3, 1, 0).height + 3;
    }
}

void erase_element(Mat &img, vector<bool> &elements, vector<Point> &Circles, vector<Point> &Lines, vector<int> &Angles,
                   vector<string> &Text_coord, vector<string> &Text_angles, bool flag) {
    if (flag == 0) {
        // Выкидываем координаты последней точки и удаляем пометку о том, что эта точка была на изображении
        Circles.pop_back();
        --number;
        Text_coord.pop_back();
        draw_point_flag = false;
    } else {
        // Выкидываем координаты концов линии и удаляем пометку о том, что эта линия была на изображении
        Text_angles.pop_back();
        Angles.pop_back();
        Lines.pop_back();
        Lines.pop_back();
        draw_point_flag = true;
    }
    elements.pop_back();
    draw_element(img, Circles, Lines);
}

void write_data_to_file(char * filename, vector<Point> &Circles, vector<Point> &Lines, vector<int> &Angles, int scale) {
    ofstream outFile(filename, ios::out);
    outFile << scale << endl;
    outFile << Circles.size() << endl;
    for (size_t i = 0; i < Circles.size(); ++i)
        outFile << Circles[i].x << " " << Circles[i].y << " " << Angles[i] << endl;
    outFile << endl;
    outFile << Lines.size() / 2 << endl;
    for (size_t i = 0; i < Lines.size(); i += 2)
        outFile << Lines[i].x << " " << Lines[i].y << " " << Lines[i + 1].x << " " << Lines[i + 1].y << endl;
    cout << "All data have been written." << endl;
}

void read_data_from_file(vector<Point> &Circles, vector<Point> &Lines, vector<int> &Angles, vector<bool> &element, int & scale) {
    ifstream inFile("Data.txt", ios::in);
    if (!inFile.is_open())
        cerr << "Can't open file!" << endl;
    inFile >> scale;
    // Читаем количество минуций
    int circles_size = 0;
    inFile >> circles_size;
    // Резервируем место
    Circles.assign(circles_size, Point(0,0));
    Angles.assign(circles_size, 0);
    // Читаем координаты минуций
    for (size_t i = 0; i < circles_size; ++i) {
        inFile >> Circles[i].x >> Circles[i].y >> Angles[i];
        Text_coord.push_back(to_string(i + 1) + ". (" + to_string(Circles[i].x) + to_string(Circles[i].y) + "), angle = ");
        Text_angles.push_back(to_string(Angles[i]));
    }
    number = circles_size;
    // Читаем количество линий
    int lines_size = 0;
    inFile >> lines_size;
    // Резервируем место
    Lines.assign(2 * lines_size, Point(0,0)); // 4 координаты для каждой линии
    // Читаем координаты линий
    for (size_t i = 0; i < 2 * lines_size; i += 2)
        inFile >> Lines[i].x >> Lines[i].y >> Lines[i + 1].x >> Lines[i + 1].y;
    element.assign(circles_size + lines_size, 0);
    // Заполняем элементы по порядку
    for (size_t i = 0; i < circles_size; ++i)
        element[i] = 0;
    for (size_t i = circles_size; i < circles_size + lines_size; ++i)
        element[i] = 1;
    cout << "All data have been read." << endl;
}

int main(int argc, char * argv[]) {
    if (argc < 2) {
        cerr << "There not enough argumetns" << endl;
        cout << "Set input and output images." << endl;
        cout << "Example: minutia_detection in.jpg out.txt" << endl;
        return -1;
    }
    cout << "Using: " << endl;
    cout << "Click by LEFT mouse button: match minutiae." << endl;
    cout << "Press DOWN by RIGHT mouse button: fix first end of tangent line." << endl;
    cout << "Moving by mouse: changing tangent direction." << endl;
    cout << "Press UP by RIGHT mouse button: fix second end of tangent line." << endl;
    cout << "Press key X: erase last drawn element." << endl;
    cout << "Press key W: write data to file - " << argv[2] << "." << endl;
    cout << "Press key Esc: exit. " << endl;
    img = imread(argv[1], CV_LOAD_IMAGE_COLOR);
    if (!img.data) {
        cerr << "Image not found" << endl;
        return -1;
    }
    copyMakeBorder(img, img, 0, 0, 0, border_size, 1, Scalar(255, 255, 255));
    line(img, Point(img.cols - border_size, 0), Point(img.cols - border_size, img.rows), Scalar(0, 0, 0));
    namedWindow("image");
    // Отлавливаем движение мыши и рисуем
    setMouseCallback("image", draw);
    // Для сохранения кода нажимаемой клавиши
    int pressed_key = 0;
    // Копируем исходное изображение в "запасное",
    // на котором будем рисовать оставшиеся элементы после удаления какого-нибудь элемента
    img.copyTo(img3);
    // Заводим полоску значений для масштабирования
    createTrackbar("Scale", "image", 0, 15);
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
            scale;
            img.copyTo(img3);
            imshow("image", img);
        }
        // Если нажали X, то убираем 1 элемент.
        if (pressed_key == 120) {
            // Если нечего удалять
            if (element.size() == 0) {
                cerr << "There are no any new elements on image" << endl;
                continue;
            }
            // Берем исходное изображение и снова рисуем на нем все, кроме последнего элемента
            img3.copyTo(img);
            // Если удаляем точку и информацию о ней
            erase_element(img, element, Circles, Lines, Angles, Text_coord, Text_angles, element.back());
            imshow("image", img);
        }
        // Если нажали R, то читаем данные из файла и отмечаем их на изображении
        if (pressed_key == 114) {
            int scale = 0;
            read_data_from_file(Circles, Lines, Angles, element, scale);
            setTrackbarPos("Scale", "image", scale);
            // Восстанавливаем "порядок" в хранении линий: i-я линия должна иметь левый конец под номером i, правый под номером i + 1;
            for (int i = 0; i < Lines.size(); i += 2) {
                if (Lines[i].x > Lines[i + 1].x) {
                    swap(Lines[i].x, Lines[i + 1].x);
                    swap(Lines[i].y, Lines[i + 1].y);
                }
            }
            if (scale > 1) resize(img, img, Size(), 1.0 + scale / 10.0, 1.0 + scale / 10.0, INTER_CUBIC);
            // Сохраняем чистое отмасштабированное изображение
            img.copyTo(img3);
            imshow("image", img);
            draw_element(img, Circles, Lines);
        }
        // Если нажали W, то записываем данные в файл и сохраняем изображение
        if (pressed_key == 119) {
            write_data_to_file(argv[2], Circles, Lines, Angles, scale);
            imwrite("Matching_image_2.jpg", img);
        }
        //cout << pressed_key << endl;
        // По нажатию Esc выходим
        if (pressed_key == 27) {
            break;
        }
    }
    return 0;
}

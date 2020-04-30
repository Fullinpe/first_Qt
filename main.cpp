#include <iostream>
#include <QApplication>
#include <synchapi.h>
#include "widget.h"

using namespace std;

#pragma clang diagnostic push
#pragma ide diagnostic ignored "EndlessLoop"
int main(int argc, char *argv[]) {
    QApplication a(argc, argv);
    Widget w;
    w.show();
    cout << "joighbk" << endl;
/*
    pthread_t pthread;
    int ret = pthread_create(&pthread, nullptr, [](void *) -> void * {
        while (true) {
            cout << "Hello" << endl;
            Sleep(500);
        }

    }, nullptr);
    if (ret != 0)
        cout << "pthread_create error: error_code=" << ret << endl;
        */
    QApplication::exec();
    return 0;
}
#pragma clang diagnostic pop

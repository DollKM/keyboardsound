#include <QApplication>
#include <QAbstractEventDispatcher>
#include <QDebug>
#include <windows.h>
#include <QSoundEffect>
#include <QUrl>
#include <QSystemTrayIcon>
#include <QAction>
#include <QMenu>

HHOOK keyHook;
QList<QSoundEffect*> mediaPlayers;

DWORD lastCode;

LRESULT CALLBACK keyProc(int nCode,WPARAM wParam,LPARAM lParam )
{
    //在WH_KEYBOARD_LL模式下lParam 是指向KBDLLHOOKSTRUCT类型地址
    //如果nCode等于HC_ACTION则处理该消息，如果小于0，则钩子子程就必须将该消息传递给 CallNextHookEx
    if(nCode == HC_ACTION){
        KBDLLHOOKSTRUCT *pkbhs = (KBDLLHOOKSTRUCT *) lParam;
        if(wParam == WM_KEYDOWN) {
//            bool bOtherKey = (GetAsyncKeyState(VK_SHIFT) < 0) ||
//                                   (GetAsyncKeyState(VK_CONTROL) < 0) ||
//                                   (GetAsyncKeyState(VK_MENU) < 0);
            if(lastCode != pkbhs->vkCode) {
                lastCode = pkbhs->vkCode;
                QSoundEffect* ptr=nullptr;
                for(int i=0;i<mediaPlayers.size();++i)
                {
                    if(mediaPlayers[i]->isPlaying()) {
                        continue;
                    }
                    ptr = mediaPlayers[i];
                    break;
                }
                if(ptr == nullptr) {
                    ptr = new QSoundEffect();
                    ptr->setSource(QUrl::fromLocalFile(QApplication::applicationDirPath()+ "/Click.wav"));
                    mediaPlayers.append(ptr);
                }
                ptr->setVolume(1.0f);
                ptr->play();
            }
        }
        else if(wParam == WM_KEYUP)
        {
            lastCode = 0;
        }
    }
    return CallNextHookEx(keyHook, nCode, wParam, lParam);
}


int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    // 创建一个新的事件分派器并安装它
    keyHook = SetWindowsHookEx( WH_KEYBOARD_LL,keyProc,nullptr,0);
    // 运行应用程序的主循环
    // 创建托盘图标对象
    QSystemTrayIcon trayIcon(QIcon(":/icon.png"), nullptr);
    
    // 创建菜单
    QMenu* trayMenu = new QMenu();
    
    QAction* quitAction = trayMenu->addAction("退出");
    QObject::connect(quitAction, &QAction::triggered, &a, &QApplication::quit);
    trayIcon.setContextMenu(trayMenu);
    trayIcon.show();
    return a.exec();
}

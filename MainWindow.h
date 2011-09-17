//////////////////////////////////////
//
// main Qt-app window and framework
// to handle module players and audio-device
//
//
// Author: Ilkka Prusi, 2011
// Contact: ilkka.prusi@gmail.com
//


#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

namespace Ui {
    class MainWindow;
}

class PlaybackHandler;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

    
private slots:
    void on_actionPlay_triggered();
    void on_actionStop_triggered();
    void on_actionFiles_triggered();

    // playback finished by player (end of file)
    void onPlaybackStopped();
	void onStatus(QString message);
    void onError(QString message);
    
private:
    Ui::MainWindow *ui;
    PlaybackHandler *m_playbackHandler;
    
    QString m_lastPath;
	QString m_baseTitle;
    
};

#endif // MAINWINDOW_H

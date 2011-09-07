#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QtMultimedia/QAudioOutput>


namespace Ui {
    class MainWindow;
}

class CModPlayer;
//class CFileType;
class CReadBuffer;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

protected:
    CModPlayer *GetPlayer(CReadBuffer &fileBuffer);
    
    
private slots:
    void on_actionPlay_triggered();
    
    void on_actionStop_triggered();
    
    void on_actionFiles_triggered();

    
private:
    Ui::MainWindow *ui;

    QAudioOutput *m_pAudioOut;
	QIODevice *m_pDevOut;
};

#endif // MAINWINDOW_H

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <vector>
#include <QSlider>
#include <QLabel>

#include "opencv2/opencv.hpp"

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

protected slots:

    void resizeEvent(QResizeEvent *event) override;
private slots:
    void on_checked_radio_botton();

    void on_top_value_vhanged(int val);
    void on_bottom_value_vhanged(int val);
    void on_LineFat_clicked();

    void on_LineSlim_clicked();

    void on_ImgReset_clicked();

    void check_handle_model(int id);
    void on_ImgTough_clicked();

    void on_ImgSoft_clicked();

    void on_ImgCoarse_clicked();

    void on_ImgSmooth_clicked();

    void on_ImgGrainy_clicked();

    void on_ImgReset_custom_clicked();

    void on_addContrastRatio_clicked();

    void on_blurBtn_clicked();

    void on_GaussianBlurBtn_clicked();

    void on_medianBlurBtn_clicked();

    void on_bilateralFilterBtn_clicked();

    void on_openBtn_clicked();

    void on_closeBtn_clicked();

    void on_dilateBtn_clicked();

    void on_erodeBtn_clicked();

    void on_ImgSharpen_clicked();

    void on_ChannelMergeBtn_stateChanged(int arg1);

private:
    Ui::MainWindow *ui;

    std::vector<cv::Mat> m_BGR_channels;
    std::vector<cv::Mat> m_HSV_channels;
    // std::vector<cv::Mat> m_YUV_channels;

    std::vector<std::vector<QSlider*>> slider_groups;

    cv::Mat m_ori_image;
    cv::Mat m_show_image;

    bool save_channel;

    // cv::Mat m_custom_image;
    // cv::Mat m_custom_show_image;

    cv::Mat m_handle_image;
    cv::Mat m_handle_show_image;

    void init_sliders_control(std::vector<QSlider *> sliders,bool init_value,bool check_state);
    void active_sliders_control(std::vector<QSlider *> sliders);
    void on_open_image();
    void custom_reset_channel_info();
    void handle_template(std::function<void ()> func);
    void reset_show_image();
    cv::Mat get_mat_with_custom_config(bool use_ori_image);
};
#endif // MAINWINDOW_H

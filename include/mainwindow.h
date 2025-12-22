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
    // 窗口尺寸变化事件
    void resizeEvent(QResizeEvent *event) override;
private slots:
    //切换 颜色空间
    void on_checked_radio_botton();
    // 顶部滑块值变化
    void on_top_value_vhanged(int val);
    // 底部滑块值变化
    void on_bottom_value_vhanged(int val);
    // 通道叠加开关
    void on_ChannelMergeBtn_stateChanged(int arg1);
    // 线条加粗
    void on_LineFat_clicked();
    //线条变细
    void on_LineSlim_clicked();
    // 风格化图像重置
    void on_ImgReset_clicked();
    // 切换操作模式（0：自定义、1：风格化）
    void check_handle_model(int id);
    // 硬朗化
    void on_ImgTough_clicked();
    // 柔和化
    void on_ImgSoft_clicked();
    // 粗糙化
    void on_ImgCoarse_clicked();
    // 光滑化
    void on_ImgSmooth_clicked();
    // 颗粒感
    void on_ImgGrainy_clicked();
    // 图像初始化（自定义风格）
    void on_ImgReset_custom_clicked();
    // 增加对比度
    void on_addContrastRatio_clicked();
    // 均值模糊
    void on_blurBtn_clicked();
    // 高斯模糊
    void on_GaussianBlurBtn_clicked();
    // 中值模糊
    void on_medianBlurBtn_clicked();
    // 双边滤波
    void on_bilateralFilterBtn_clicked();
    // 开操作 先腐蚀后膨胀 适合去除脏点
    void on_openBtn_clicked();
    // 关操作 先膨胀后腐蚀 适合链接轮廓
    void on_closeBtn_clicked();
    // 膨胀操作
    void on_dilateBtn_clicked();
    // 腐蚀操作
    void on_erodeBtn_clicked();
    // 锐化操作
    void on_ImgSharpen_clicked();


private:
    Ui::MainWindow *ui;
    //存放 通道内容
    std::vector<cv::Mat> m_BGR_channels;
    std::vector<cv::Mat> m_HSV_channels;

    // RGB、HSV、YUV按钮列表
    std::vector<std::vector<QSlider*>> m_slider_groups;
    // 真正的原图
    cv::Mat m_ori_image;
    // 自定义处理状态图
    cv::Mat m_show_image;
    // 风格化操作图及展示图
    cv::Mat m_handle_image;
    cv::Mat m_handle_show_image;

    //风格操作图初始化时是否来自自定义图当前状态
    bool m_save_channel;

    ///
    /// \brief init_sliders_control 初始化按钮
    /// \param sliders 按钮列表
    /// \param init_value 值是否归零
    /// \param check_state 是否禁止按钮
    ///
    void init_sliders_control(std::vector<QSlider *> sliders,bool init_value,bool check_state);
    ///
    /// \brief active_sliders_control 激活按钮状态
    /// \param sliders 按钮列表
    ///
    void active_sliders_control(std::vector<QSlider *> sliders);
    ///
    /// \brief on_open_image 打开图片
    ///
    void on_open_image();
    ///
    /// \brief custom_reset_channel_info 从m_show_image重置通道内容
    ///
    void custom_reset_channel_info();
    ///
    /// \brief handle_template 处理风格操作的函数模板
    /// \param func 嵌入lamda函数
    ///
    void handle_template(std::function<void ()> func);
    ///
    /// \brief reset_show_image 从原图重置m_show_image
    ///
    void reset_show_image();
    ///
    /// \brief get_mat_with_custom_bottom_config 根据叠加状态的滑块状态生成cv::Mat
    ///             （底部9个滑块）
    /// \param use_ori_image
    ///             true：从原图重新取通道
    ///             false：从RGB、YUV、HSV通道修改
    /// \return
    ///
    cv::Mat get_mat_with_custom_bottom_config(bool use_ori_image);
    ///
    /// \brief get_mat_with_custom_config 根据非叠加状态的滑块状态生成cv::Mat
    ///             （顶部9个滑块）
    /// \param use_ori_image
    ///             true：从原图重新取通道
    ///             false：从RGB、YUV、HSV通道修改
    /// \return
    ///
    cv::Mat get_mat_with_custom_config(bool use_ori_image);
};
#endif // MAINWINDOW_H

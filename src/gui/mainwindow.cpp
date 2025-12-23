#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QFileDialog>
#include <QStandardPaths>
#include <QAction>
#include <fstream>
#include <QMessageBox>
#include <QButtonGroup>
///
///\brief Mat2QImage opencv格式Mat转QImage
///\param mat
///\return
///
static QImage Mat2QImage(const cv::Mat &mat) {
    if (mat.type() == CV_8UC3) {
        const uchar *pSrc = const_cast<const uchar *>(mat.data);
        QImage image(pSrc, mat.cols, mat.rows, static_cast<qint32>(mat.step), QImage::Format_RGB888);
        return image.rgbSwapped();
    } else if (mat.type() == CV_8UC4) {
        const uchar *pSrc = const_cast<const uchar *>(mat.data);
        QImage image(pSrc, mat.cols, mat.rows, static_cast<qint32>(mat.step), QImage::Format_ARGB32);
        return image.copy();
    } else{
        //不正确的格式
        return QImage();
    }
}



MainWindow::MainWindow(QWidget *parent): QMainWindow(parent), ui(new Ui::MainWindow){
    ui->setupUi(this);

    m_save_channel = true;

    ui->ori_image->setAlignment(Qt::AlignCenter);
    ui->show_image->setAlignment(Qt::AlignCenter);

    //打开图片
    connect(ui->openImage,&QAction::triggered,this,&MainWindow::on_open_image);

    //颜色空间按钮切换 信号绑定
    connect(ui->RGBBtn,&QRadioButton::clicked,this,&MainWindow::on_checked_radio_botton);
    connect(ui->HSVBtn,&QRadioButton::clicked,this,&MainWindow::on_checked_radio_botton);
    connect(ui->YUVBtn,&QRadioButton::clicked,this,&MainWindow::on_checked_radio_botton);

    //非叠加状态（顶部） 9 个滑块值变化 信号绑定
    connect(ui->RSlider,&QSlider::valueChanged,this,&MainWindow::on_top_value_vhanged);
    connect(ui->GSlider,&QSlider::valueChanged,this,&MainWindow::on_top_value_vhanged);
    connect(ui->BSlider,&QSlider::valueChanged,this,&MainWindow::on_top_value_vhanged);
    connect(ui->HSlider,&QSlider::valueChanged,this,&MainWindow::on_top_value_vhanged);
    connect(ui->SSlider,&QSlider::valueChanged,this,&MainWindow::on_top_value_vhanged);
    connect(ui->VSlider,&QSlider::valueChanged,this,&MainWindow::on_top_value_vhanged);
    connect(ui->YSlider,&QSlider::valueChanged,this,&MainWindow::on_top_value_vhanged);
    connect(ui->USlider,&QSlider::valueChanged,this,&MainWindow::on_top_value_vhanged);
    connect(ui->V_2Slider,&QSlider::valueChanged,this,&MainWindow::on_top_value_vhanged);
    //索引绑定
    ui->RSlider->setProperty("bindLabelIdx",1);
    ui->GSlider->setProperty("bindLabelIdx",2);
    ui->BSlider->setProperty("bindLabelIdx",3);
    ui->HSlider->setProperty("bindLabelIdx",4);
    ui->SSlider->setProperty("bindLabelIdx",5);
    ui->VSlider->setProperty("bindLabelIdx",6);
    ui->YSlider->setProperty("bindLabelIdx",7);
    ui->USlider->setProperty("bindLabelIdx",8);
    ui->V_2Slider->setProperty("bindLabelIdx",9);


    //叠加状态（底部） 9 个滑块值变化 信号绑定
    connect(ui->firstSlider,&QSlider::valueChanged,this,&MainWindow::on_bottom_value_vhanged);
    connect(ui->secondSlider,&QSlider::valueChanged,this,&MainWindow::on_bottom_value_vhanged);
    connect(ui->thirdSlider,&QSlider::valueChanged,this,&MainWindow::on_bottom_value_vhanged);
    connect(ui->firstSlider_2,&QSlider::valueChanged,this,&MainWindow::on_bottom_value_vhanged);
    connect(ui->secondSlider_2,&QSlider::valueChanged,this,&MainWindow::on_bottom_value_vhanged);
    connect(ui->thirdSlider_2,&QSlider::valueChanged,this,&MainWindow::on_bottom_value_vhanged);
    connect(ui->firstSlider_3,&QSlider::valueChanged,this,&MainWindow::on_bottom_value_vhanged);
    connect(ui->secondSlider_3,&QSlider::valueChanged,this,&MainWindow::on_bottom_value_vhanged);
    connect(ui->thirdSlider_3,&QSlider::valueChanged,this,&MainWindow::on_bottom_value_vhanged);
    //索引绑定
    ui->firstSlider->setProperty("bindLabelIdx",1);
    ui->secondSlider->setProperty("bindLabelIdx",2);
    ui->thirdSlider->setProperty("bindLabelIdx",3);
    ui->firstSlider_2->setProperty("bindLabelIdx",4);
    ui->secondSlider_2->setProperty("bindLabelIdx",5);
    ui->thirdSlider_2->setProperty("bindLabelIdx",6);
    ui->firstSlider_3->setProperty("bindLabelIdx",7);
    ui->secondSlider_3->setProperty("bindLabelIdx",8);
    ui->thirdSlider_3->setProperty("bindLabelIdx",9);

    //操作模式切换按钮初始化及信号绑定
    QButtonGroup* check_btns = new QButtonGroup(this);
    check_btns->addButton(ui->CustomConfigBtn,0);
    check_btns->addButton(ui->FastHandleBtn,1);
    check_btns->setExclusive(true);
    check_btns->button(0)->setChecked(true);
    connect(check_btns,&QButtonGroup::idClicked,this,&MainWindow::check_handle_model);

    //方便遍历更改按钮状态 (非叠加态)
    m_slider_groups = std::vector<std::vector<QSlider*>>({
        {ui->RSlider,ui->GSlider,ui->BSlider},
        {ui->HSlider,ui->SSlider,ui->VSlider},
        {ui->YSlider,ui->USlider,ui->V_2Slider}
    });
    on_checked_radio_botton();

}

//切换操作模式
void MainWindow::check_handle_model(int id){
    switch (id) {
    case 0:
        //开启滑块所属窗体
        this->ui->custom_frame->setDisabled(false);
        this->ui->fast_handle_frame->setDisabled(true);
        //释放掉风格化操作图 需要时还要去创建
        m_handle_show_image.release();
        m_handle_image.release();

        if(!m_ori_image.empty()){
            reset_show_image();
            //初始化相关通道
            custom_reset_channel_info();
            //根据设定值展示图象
            ui->ChannelMergeBtn->isChecked() ? on_bottom_value_vhanged(100) : on_top_value_vhanged(100);
        }

        break;
    case 1:
        this->ui->custom_frame->setDisabled(true);
        this->ui->fast_handle_frame->setDisabled(false);
        on_ImgReset_clicked();
        break;
    default:
        break;
    }
}

void MainWindow::reset_show_image(){
    //计算缩放因子
    float scale = std::min((float)(ui->show_image->width())/(float)(m_ori_image.cols),(float)(ui->show_image->height())/(float)(m_ori_image.rows));
    cv::resize(m_ori_image,m_show_image,cv::Size(0,0),scale,scale,scale >0 ? cv::INTER_CUBIC : cv::INTER_AREA);
    QImage img = Mat2QImage(m_show_image);
    //展示 图片
    ui->ori_image->setPixmap(QPixmap::fromImage(img));
}

void MainWindow::custom_reset_channel_info(){
    if(m_show_image.empty()) return;
    //准备RGB
    cv::split(m_show_image,m_BGR_channels);
    //准备HSV
    cv::Mat hsv_img;
    cv::cvtColor(m_show_image,hsv_img,cv::COLOR_BGR2HSV);
    cv::split(hsv_img,m_HSV_channels);

}

void MainWindow::init_sliders_control(std::vector<QSlider*> sliders,bool init_value,bool check_state){
    foreach (auto slider, sliders) {
        if(init_value)
            slider->setValue(0);
        if (check_state)
            slider->setDisabled(true);
    }
}

void MainWindow::active_sliders_control(std::vector<QSlider*> sliders){
    foreach (auto slider, sliders) {
        slider->setDisabled(false);
    }
}

//切换色彩空间
void MainWindow::on_checked_radio_botton(){

    std::vector<QString> channels;
    //按钮先全部禁用 再根据radioBtn的选择单独激活
    foreach (auto& sliders, m_slider_groups) {
        init_sliders_control(sliders,false,true);
    }
    //激活 RGB
    if(ui->RGBBtn->isChecked()){
        active_sliders_control( m_slider_groups[0]);
        channels = {"R","G","B"};
    }
    //激活 HSV
    if(ui->HSVBtn->isChecked()){
        active_sliders_control( m_slider_groups[1]);
        channels = {"H","S","V"};
    }
    //激活 YUV
    if(ui->YUVBtn->isChecked()){
        active_sliders_control( m_slider_groups[2]);
        if(ui->ChannelMergeBtn->isChecked()){
            QMessageBox::warning(this,"警告","YUV格式不支持通道叠加");
            ui->ChannelMergeBtn->setCheckState(Qt::CheckState::Unchecked);
        }
    }
    //叠加状态文本修改
    if(channels.size()>0){
        ui->first->setText(channels[0]);
        ui->firstLabel->setText(channels[0]);
        ui->firstLabel_2->setText(channels[0]);
        ui->firstLabel_3->setText(channels[0]);
        ui->second->setText(channels[1]);
        ui->secondLabel->setText(channels[1]);
        ui->secondLabel_2->setText(channels[1]);
        ui->secondLabel_3->setText(channels[1]);
        ui->third->setText(channels[2]);
        ui->thirdLabel->setText(channels[2]);
        ui->thirdLabel_2->setText(channels[2]);
        ui->thirdLabel_3->setText(channels[2]);
    }
    //重置下方图片 由于没有sender bindLabelIdx 为 0 所以100无含义，没有影响
    ui->ChannelMergeBtn->isChecked() ? on_bottom_value_vhanged(100) : on_top_value_vhanged(100);

}


cv::Mat MainWindow::get_mat_with_custom_config(bool use_ori_image){
    auto start = clock();
    auto source_channel = m_BGR_channels;
    if(use_ori_image){
        cv::split(m_ori_image,source_channel);
    }

    //根据 RGB 滑块操作图片
    cv::Mat show_img;
    std::vector<cv::Mat> rgb_show_channels;
    for (int i = 0; i < source_channel.size(); ++i) {
        cv::Mat c = source_channel[i].clone();
        switch (i) {
        case 0:
            //B
            c*=(1.0f+(float)(ui->BSlider->value())/100.0f);
            break;
        case 1:
            //G
            c*=(1.0f+(float)(ui->GSlider->value())/100.0f);
            break;
        case 2:
            //R
            c*=(1.0f+(float)(ui->RSlider->value())/100.0f);
            break;
        default:
            break;
        }
        rgb_show_channels.push_back(c);
    }
    cv::merge(rgb_show_channels,show_img);

    //根据 HSV 滑块操作图片
    cv::cvtColor(show_img,show_img,cv::COLOR_BGR2HSV);
    std::vector<cv::Mat> HSV_channels;
    cv::split(show_img,HSV_channels);
    std::vector<cv::Mat> hsv_show_channels;
    for (int i = 0; i < HSV_channels.size(); ++i) {
        cv::Mat c = HSV_channels[i].clone();
        switch (i) {
        case 0:
            //H
            c*=(1+(float)(ui->HSlider->value())/100.0f);
            break;
        case 1:
            //S
            c*=(1+(float)(ui->SSlider->value())/100.0f);
            break;
        case 2:
            //V
            c*=(1+(float)(ui->VSlider->value())/100.0f);
            break;
        default:
            break;
        }
        hsv_show_channels.push_back(c);
    }
    cv::merge(hsv_show_channels,show_img);
    cv::cvtColor(show_img,show_img,cv::COLOR_HSV2BGR);

    //根据 YUV 滑块操作图片
    cv::cvtColor(show_img,show_img,cv::COLOR_BGR2YUV);
    std::vector<cv::Mat> YUV_channels;
    cv::split(show_img,YUV_channels);
    std::vector<cv::Mat> yuv_show_channels;
    for (int i = 0; i < YUV_channels.size(); ++i) {
        cv::Mat c = YUV_channels[i].clone();
        switch (i) {
        case 0:
            //H
            c*=(1.0f+((float)(ui->YSlider->value())/100.0f));
            break;
        case 1:
            //S
            c*=(1.0f+((float)(ui->USlider->value())/100.0f));
            break;
        case 2:
            //V
            c*=(1.0f+((float)(ui->V_2Slider->value())/100.0f));
            break;
        default:
            break;
        }
        yuv_show_channels.push_back(c);
    }
    cv::merge(yuv_show_channels,show_img);
    cv::cvtColor(show_img,show_img,cv::COLOR_YUV2BGR);
    auto end = clock();
    qDebug() << "RGB+HSV+YUV Use Time: " << QString::number(end-start) << "ms";
    return show_img;

}

cv::Mat MainWindow::get_mat_with_custom_bottom_config(bool use_ori_image){
    std::vector<cv::Mat> source_channel;

    if(ui->RGBBtn->isChecked()){
        source_channel = m_BGR_channels;
    }
    if(ui->HSVBtn->isChecked()){
        source_channel = m_HSV_channels;
    }

    if(use_ori_image){
        if(ui->RGBBtn->isChecked()){
            cv::split(m_ori_image,source_channel);
        }
        if(ui->HSVBtn->isChecked()){
            cv::Mat hsv_img;
            cv::cvtColor(m_ori_image,hsv_img,cv::COLOR_BGR2HSV);
            cv::split(hsv_img,source_channel);
        }
    }
    std::vector<cv::Mat> show_channels;
    for (int i = 0; i < source_channel.size(); ++i) {
        cv::Mat c = source_channel[i].clone();
        switch (i) {
        case 0:
            //B
            c= ((float)(ui->firstSlider->value())/100.0f)*c + ((float)(ui->secondSlider->value())/100.0f)*c + ((float)(ui->thirdSlider->value())/100.0f)*c;
            break;
        case 1:
            //G
            c=((float)(ui->firstSlider_2->value())/100.0f)*c + ((float)(ui->secondSlider_2->value())/100.0f)*c + ((float)(ui->thirdSlider_2->value())/100.0f)*c;
            break;
        case 2:
            //R
            c=((float)(ui->firstSlider_3->value())/100.0f)*c + ((float)(ui->secondSlider_3->value())/100.0f)*c + ((float)(ui->thirdSlider_3->value())/100.0f)*c;
            break;
        default:
            break;
        }
        show_channels.push_back(c);
    }
    cv::Mat show_img;
    cv::merge(show_channels,show_img);
    if(ui->HSVBtn->isChecked()){
        cv::cvtColor(show_img,show_img,cv::COLOR_HSV2RGB);
    }
    if(ui->RGBBtn->isChecked()){
        cv::cvtColor(show_img,show_img,cv::COLOR_BGR2RGB);
    }
    return show_img;
}
//非颜色叠加色值变动
void MainWindow::on_top_value_vhanged(int val){
    float num = val/100.0f;
    char buffer[50];
    sprintf(buffer, "%.2f", num);
    //根据滑块 绑定的索引 修改对应右侧文本（非叠加状态）
    if(sender()){
        switch (sender()->property("bindLabelIdx").toInt()) {
        case 1:
            ui->RValue->setText(buffer);
            break;
        case 2:
            ui->GValue->setText(buffer);
            break;
        case 3:
            ui->BValue->setText(buffer);
            break;
        case 4:
            ui->HValue->setText(buffer);
            break;
        case 5:
            ui->SValue->setText(buffer);
            break;
        case 6:
            ui->VValue->setText(buffer);
            break;
        case 7:
            ui->YValue->setText(buffer);
            break;
        case 8:
            ui->UValue->setText(buffer);
            break;
        case 9:
            ui->V_2Value->setText(buffer);
            break;
        default:
            break;
        }
    }

    if(m_ori_image.empty()) return;
    auto show_img = get_mat_with_custom_config(false);
    QImage img = Mat2QImage(show_img);
    ui->show_image->setPixmap(QPixmap::fromImage(img));
}


//颜色叠加色值变动
void MainWindow::on_bottom_value_vhanged(int val){
    float num = val/100.0f;
    char buffer[50];
    sprintf(buffer, "%.2f", num);
    //根据滑块 绑定的索引 修改对应右侧文本（叠加状态）
    if(sender()){
        switch (sender()->property("bindLabelIdx").toInt()) {
        case 1:
            ui->firstValue->setText(buffer);
            break;
        case 2:
            ui->secondValue->setText(buffer);
            break;
        case 3:
            ui->thirdValue->setText(buffer);
            break;
        case 4:
            ui->firstValue_2->setText(buffer);
            break;
        case 5:
            ui->secondValue_2->setText(buffer);
            break;
        case 6:
            ui->thirdValue_2->setText(buffer);
            break;
        case 7:
            ui->firstValue_3->setText(buffer);
            break;
        case 8:
            ui->secondValue_3->setText(buffer);
            break;
        case 9:
            ui->thirdValue_3->setText(buffer);
            break;
        default:
            break;
        }
    }

    if(m_ori_image.empty()) return;
    cv::Mat show_img = get_mat_with_custom_bottom_config(false);
    QImage img = Mat2QImage(show_img);
    ui->show_image->setPixmap(QPixmap::fromImage(img));
}


void MainWindow::resizeEvent(QResizeEvent *event)
{
    //调整控件位置
    ui->config_frame->setFixedSize(this->width(),320);
    ui->config_frame->move(0,0);

    ui->show_frame->move(0,ui->config_frame->y() + ui->config_frame->height());
    ui->show_frame->setFixedSize(this->width(),height() - ui->show_frame->y()-ui->menuBar->height());

    if(!m_ori_image.empty()){
        //先对原图图像进行 resize
        reset_show_image();

        //自定义操作图:m_show_image
        if(ui->CustomConfigBtn->isChecked()){
            //初始化相关通道
            custom_reset_channel_info();
            //根据设定值展示图象
            ui->ChannelMergeBtn->isChecked() ? on_bottom_value_vhanged(100) : on_top_value_vhanged(100);
        }
        //风格化操作图:m_handle_image;
        if(ui->FastHandleBtn->isChecked()){
            //调整尺寸 不重置
            float scale = std::min((float)(ui->show_image->width())/(float)(m_handle_image.cols),(float)(ui->show_frame->height())/(float)(m_handle_image.rows));
            cv::resize(m_handle_image,m_handle_show_image,cv::Size(0,0),scale,scale,scale >0 ? cv::INTER_CUBIC : cv::INTER_AREA);
            QImage img = Mat2QImage(m_handle_show_image);
            ui->show_image->setPixmap(QPixmap::fromImage(img));
        }
    }
}

//打开图片
void MainWindow::on_open_image(){
    QString file_url = QFileDialog::getOpenFileName(this,u8"打开图片",QStandardPaths::writableLocation(QStandardPaths::DesktopLocation),"图片文件(*.jpg *.png)");
    std::ifstream file(std::filesystem::u8path(file_url.toUtf8().toStdString()), std::ios::binary);
    if (!file.is_open()) {
        std::cerr << "Error: [imread_unicode] can't open file..." << std::endl;
        return;
    }
    const std::vector<char> buffer(std::istreambuf_iterator<char>(file), {});
    file.close();
    if (buffer.empty()) {
        std::cerr << "Error: [imread_unicode] file is empty...: " << std::endl;
        return;
    }
    try {
        //根据文件流中的数据解码图片
        m_ori_image = cv::imdecode(buffer,cv::IMREAD_COLOR);
        if (m_ori_image.empty()) {
            std::cerr << "Error: [imread_unicode] cv::imdecode failed... " << std::endl;
        }

    } catch (const cv::Exception& ex) {
        std::cerr << "Error: [imread_unicode] cv::imdecode crashed... " << ex.what() << std::endl;
        return;
    }
    reset_show_image();


    if(ui->CustomConfigBtn->isChecked()){
        //初始化相关通道
        custom_reset_channel_info();
        //根据设定值展示图象
        ui->ChannelMergeBtn->isChecked() ? on_bottom_value_vhanged(100) : on_top_value_vhanged(100);
    }
    if(ui->FastHandleBtn->isChecked()){
        //初始化 滤镜图像
        on_ImgReset_clicked();
    }
}

void MainWindow::on_ImgReset_clicked()
{
    if(!m_ori_image.empty()){
        //重新根据滑块数值获取图像 m_save_channel当前只会为 true
        if(m_save_channel){
            auto show_img = ui->ChannelMergeBtn->isChecked() ? get_mat_with_custom_bottom_config(true) : get_mat_with_custom_config(true);
            m_handle_image = show_img.clone();
        }else{
            m_handle_image = m_ori_image.clone();
        }
        float scale = std::min((float)(ui->show_image->width())/(float)(m_handle_image.cols),(float)(ui->show_image->height())/(float)(m_handle_image.rows));
        cv::resize(m_handle_image,m_handle_show_image,cv::Size(0,0),scale,scale,scale >0 ? cv::INTER_CUBIC : cv::INTER_AREA);
        QImage img = Mat2QImage(m_handle_show_image);
        ui->show_image->setPixmap(QPixmap::fromImage(img));
    }
}

void MainWindow::handle_template(std::function<void(void)> func){

    auto start = clock();
    if(m_handle_image.empty()) return;

    //针对m_handle_image进行操作
    func();

    //操作完重新显示回窗体
    float scale = std::min((float)(ui->show_image->width())/(float)(m_handle_image.cols),(float)(ui->show_image->height())/(float)(m_handle_image.rows));
    cv::resize(m_handle_image,m_handle_show_image,cv::Size(0,0),scale,scale,scale >0 ? cv::INTER_CUBIC : cv::INTER_AREA);
    QImage img = Mat2QImage(m_handle_show_image);
    ui->show_image->setPixmap(QPixmap::fromImage(img));
    auto end = clock();
    qDebug() << "RGB+HSV+YUV Use Time: " << QString::number(end-start) << "ms";

}




void MainWindow::on_ChannelMergeBtn_stateChanged(int arg1)
{
    switch (arg1) {
    case Qt::CheckState::Checked:
        ui->top->setDisabled(true);
        ui->bottom->setDisabled(false);
        break;
    case Qt::CheckState::Unchecked:
        ui->top->setDisabled(false);
        ui->bottom->setDisabled(true);
        break;
    default:
        break;
    }
    on_checked_radio_botton();

}



//线条加粗
void MainWindow::on_LineFat_clicked()
{
    handle_template([this](){
        cv::Mat gray;
        cv::cvtColor(m_handle_image,gray,cv::COLOR_BGR2GRAY);
        cv::Mat edges;
        //获取轮廓信息
        cv::Canny(gray,edges,100,200,3,true);
        cv::Mat element = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(5, 5));
        //使用闭操作连接相近零碎轮廓
        cv::morphologyEx(edges,edges,cv::MORPH_CLOSE,element);
        //画笔颜色变淡
        cv::threshold(edges,edges,150,30,cv::THRESH_BINARY);
        //羽化画笔线条硬度
        cv::GaussianBlur(edges,edges,cv::Size(13,13),0);
        cv::Mat addImg;
        cv::cvtColor(edges,addImg,cv::COLOR_GRAY2BGR);
        //通过相减画到原图上
        m_handle_image -= addImg;

    });
}
//        ↑
//  仅在画的时候不同
//        ↓
//线条变细
void MainWindow::on_LineSlim_clicked()
{
    handle_template([this](){
        cv::Mat gray;
        cv::cvtColor(m_handle_image,gray,cv::COLOR_BGR2GRAY);
        cv::Mat edges;
        //获取轮廓信息
        cv::Canny(gray,edges,100,200,3,true);
        cv::Mat element = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(5, 5));
        //使用闭操作连接相近零碎轮廓
        cv::morphologyEx(edges,edges,cv::MORPH_CLOSE,element);
        //画笔颜色变淡
        cv::threshold(edges,edges,150,30,cv::THRESH_BINARY);
        //羽化画笔线条硬度
        cv::GaussianBlur(edges,edges,cv::Size(13,13),0);
        cv::Mat addImg;
        cv::cvtColor(edges,addImg,cv::COLOR_GRAY2BGR);
        //通过相加画到原图上
        m_handle_image += addImg;
    });
}

//

void MainWindow::on_ImgTough_clicked()
{
    handle_template([this](){
        qDebug() << __FUNCTION__;
    });
}


void MainWindow::on_ImgSoft_clicked()
{
    handle_template([this](){
        qDebug() << __FUNCTION__;
    });
}


//图像粗糙
void MainWindow::on_ImgCoarse_clicked()
{
    handle_template([this](){

        cv::Mat noise(m_handle_image.size(), CV_8UC1);
        double mean = -10;
        double dev = 35;
        cv::randn(noise, mean, dev); //生成高斯噪声
        //使用黑白噪点
        cv::cvtColor(noise,noise,cv::COLOR_GRAY2BGR);
        cv::blur(noise, noise,cv::Size(3,3));
        //原图和噪点相加
        noise += m_handle_image;
        //根据权重添加噪声
        cv::addWeighted(m_handle_image,0.6,noise,0.4,0,m_handle_image);

    });
}

//图像光滑
void MainWindow::on_ImgSmooth_clicked()
{
    handle_template([this](){
        cv::Mat gray;
        cv::cvtColor(m_handle_image,gray,cv::COLOR_BGR2GRAY);
        cv::Mat edges;
        cv::Canny(gray,edges,100,200,3,true);
        cv::Mat element = cv::getStructuringElement(cv::MORPH_ELLIPSE, cv::Size(5, 5));
        //连接附近轮廓
        cv::morphologyEx(edges,edges,cv::MORPH_CLOSE,element);
        //扩散轮廓边缘
        cv::Mat kernel = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(3, 3));
        cv::dilate(edges, edges, kernel);
        //对原图进行高斯模糊
        cv::Mat blurred;
        cv::GaussianBlur(m_handle_image, blurred, cv::Size(3, 3), 0);
        //把线条部分进行高斯模糊的部分通过mask copy到原图进行替换
        blurred.copyTo(m_handle_image, edges);
});
}

//颗粒滤镜 增加随机颜色噪点
void MainWindow::on_ImgGrainy_clicked()
{
    handle_template([this](){
        cv::Mat noise(m_handle_image.size(), CV_8UC3);
        double mean = 0;
        double dev = 10;
        cv::randn(noise, mean, dev);
        m_handle_image += noise;
    });
}

//增加对比度 整体颜色提升
void MainWindow::on_addContrastRatio_clicked()
{
    handle_template([this](){
        cv::convertScaleAbs(m_handle_image, m_handle_image,1.1);
    });
}

void MainWindow::on_ImgReset_custom_clicked()
{
    if(!m_ori_image.empty()){
        if(ui->ChannelMergeBtn->isChecked()){
            ui->firstSlider->setValue(0);
            ui->firstSlider_2->setValue(0);
            ui->firstSlider_3->setValue(0);
            ui->secondSlider->setValue(0);
            ui->secondSlider_2->setValue(0);
            ui->secondSlider_3->setValue(0);
            ui->thirdSlider->setValue(0);
            ui->thirdSlider_2->setValue(0);
            ui->thirdSlider_3->setValue(0);
        }else{
            reset_show_image();
            //初始化自定义展示图片
            //m_custom_image = m_show_image.clone();
            //初始化相关通道
            custom_reset_channel_info();
            //根据设定值展示图象
            ui->ChannelMergeBtn->isChecked() ? on_bottom_value_vhanged(100) : on_top_value_vhanged(100);

            //QImage img = Mat2QImage(m_show_image);
            //ui->show_image->setPixmap(QPixmap::fromImage(img));
            foreach (auto group, m_slider_groups) {
                init_sliders_control(group,true,false);
            }
        }

    }
}


void MainWindow::on_blurBtn_clicked()
{
    handle_template([this](){
        cv::blur(m_handle_image, m_handle_image,cv::Size(3,3));
    });
}


void MainWindow::on_medianBlurBtn_clicked()
{
    handle_template([this](){
        cv::medianBlur(m_handle_image, m_handle_image,3);
    });
}


void MainWindow::on_GaussianBlurBtn_clicked()
{
    handle_template([this](){
        cv::GaussianBlur(m_handle_image, m_handle_image,cv::Size(3,3),0);
    });
}


void MainWindow::on_bilateralFilterBtn_clicked()
{
    handle_template([this](){
        cv::Mat dst;
        cv::bilateralFilter(m_handle_image, dst,0,11,11);
        m_handle_image = dst.clone();
    });
}


void MainWindow::on_openBtn_clicked()
{
    handle_template([this](){
        cv::Mat element = cv::getStructuringElement(cv::MORPH_ELLIPSE, cv::Size(5, 5));
        cv::morphologyEx(m_handle_image,m_handle_image,cv::MORPH_OPEN,element);
    });
}


void MainWindow::on_closeBtn_clicked()
{
    handle_template([this](){
        cv::Mat element = cv::getStructuringElement(cv::MORPH_ELLIPSE, cv::Size(5, 5));
        cv::morphologyEx(m_handle_image,m_handle_image,cv::MORPH_CLOSE,element);
    });
}


void MainWindow::on_dilateBtn_clicked()
{
    handle_template([this](){
        cv::Mat element = cv::getStructuringElement(cv::MORPH_ELLIPSE, cv::Size(5, 5));
        cv::morphologyEx(m_handle_image,m_handle_image,cv::MORPH_DILATE,element);
    });
}


void MainWindow::on_erodeBtn_clicked()
{
    handle_template([this](){
        cv::Mat element = cv::getStructuringElement(cv::MORPH_ELLIPSE, cv::Size(5, 5));
        cv::morphologyEx(m_handle_image,m_handle_image,cv::MORPH_ERODE,element);
    });
}


void MainWindow::on_ImgSharpen_clicked()
{
    handle_template([this](){
            //锐化
            handle_template([this](){
                cv::Mat blur_usm;
                cv::GaussianBlur(m_handle_image, blur_usm, cv::Size(0, 0), 5.0);
                cv::addWeighted(m_handle_image, 1.5, blur_usm,-0.5, 0, m_handle_image);
            });
    });
}


//void MainWindow::on_ImgSharpen_clicked()
//{
//    handle_template([this](){
//        cv::Mat blur_usm;
//        cv::GaussianBlur(m_handle_image, blur_usm, cv::Size(0, 0), 9);
//        cv::addWeighted(m_handle_image, 1.5, blur_usm, -0.5, 0, m_handle_image);
//    });
//}

MainWindow::~MainWindow()
{
    delete ui;
}



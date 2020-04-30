#include <QtGui/QPainter>
#include "widget.h"
#include "ui_widget.h"
//#include "AvUtils.h"

extern "C" {
#include <swscale.h>
#include <imgutils.h>
#include <avformat.h>
#include <avdevice.h>
}
using namespace std;

static QImage image;


uint8_t *yuvBuffer;
AVFrame *pFrame;
AVFrame *pFrameRGB;
uint8_t *rgbBuffer;
SwsContext *img_convert_ctx;

//初始化
void InitPlay(int width, int height) {
    //width和heigt为传入的分辨率的大小，实际应用我传的1280*720
    int yuvSize = width * height * 3 / 2;
    yuvBuffer = (uint8_t *) malloc(yuvSize);
    //为每帧图像分配内存
    pFrame = av_frame_alloc();
    pFrameRGB = av_frame_alloc();
    int numBytes = avpicture_get_size(AV_PIX_FMT_RGB32, width, height);
    rgbBuffer = (uint8_t *) av_malloc(numBytes * sizeof(uint8_t));
    avpicture_fill((AVPicture *) pFrameRGB, rgbBuffer, AV_PIX_FMT_RGB32, width, height);
    //特别注意 img_convert_ctx 该在做H264流媒体解码时候，发现sws_getContext /sws_scale内存泄露问题，
    //注意sws_getContext只能调用一次，在初始化时候调用即可，另外调用完后，在析构函数中使用sws_free_Context，将它的内存释放。
    //设置图像转换上下文
    img_convert_ctx = sws_getContext(width, height, AV_PIX_FMT_YUV420P, width, height, AV_PIX_FMT_RGB32, SWS_BICUBIC, NULL,
                                     NULL, NULL);
}

//实时接收yuv数据转换成rgb32 str是yuv数据，len为长度
void wb_yuv(void *str, int len) {
    int width=1280,height=720;
    avpicture_fill((AVPicture *) pFrame, (uint8_t *) str, AV_PIX_FMT_YUV420P, width, height);//这里的长度和高度跟之前保持一致
    //转换图像格式，将解压出来的YUV420P的图像转换为RGB的图像
    sws_scale(img_convert_ctx,
              (uint8_t const *const *) pFrame->data,
              pFrame->linesize, 0, height, pFrameRGB->data,
              pFrameRGB->linesize);
    //把这个RGB数据 用QImage加载
//    QImage tmpImg((uchar *) rgbBuffer, mWidth, mHeight, QImage::Format_RGB32);
    QImage tmpImg((uchar *) rgbBuffer, width, height, QImage::Format_RGB32);
    image = tmpImg.copy(); //把图像复制一份 传递给界面显示
    //todo:不懂
//    emit sig_GetOneFrame(image);  //发送信号，将次imamge发送到要显示的控件，用paintEvent绘制出来

}


Widget::Widget(QWidget *parent)
        : QWidget(parent), ui(new Ui::Widget) {

    ui->setupUi(this);
    connect(ui->mbutton, &QPushButton::clicked, this, [=]() {
                static int x = 0;
                x++;
                ui->progressBar->setValue(x);

            }
    );

    avdevice_register_all();

    AVFormatContext *cam = nullptr;
    AVInputFormat *iformat = av_find_input_format("dshow");
    string url = "video=USB2.0 HD UVC WebCam:audio=麦克风 (Realtek High Definition Audio)";
    if (avformat_open_input(&cam, url.c_str(), iformat, nullptr) != 0) {
        printf("Couldn't open input stream.\n");
    }
    /* retrieve stream information */
    if (avformat_find_stream_info(cam, nullptr) < 0) {
        fprintf(stderr, "Could not find stream information\n");
        exit(1);
    }

//    AVFormatContext *cam = AvUtils::openCamera("video=USB2.0 HD UVC WebCam:audio=麦克风 (Realtek High Definition Audio)");

    if (cam) {   //---------------------------------------------

        int videoindex = -1;
        for (int i = 0; i < cam->nb_streams; i++) {
            if (cam->streams[i]->codec->codec_type == AVMEDIA_TYPE_VIDEO) {
                videoindex = i;
            }
        }
        if (videoindex == -1) {
            printf("Couldn't find a video stream.\n");
            goto label_END;
        }
        AVCodecContext *pCodecCtx = cam->streams[videoindex]->codec;
        AVCodec *pCodec = avcodec_find_decoder(pCodecCtx->codec_id);
        if (pCodec == nullptr) {
            printf("Codec not found.\n");
            goto label_END;
        }
        if (avcodec_open2(pCodecCtx, pCodec, nullptr) < 0) {
            printf("Could not open codec.\n");
            goto label_END;
        }
        AVFrame *pFrame, *pFrameYUV;
        pFrame = av_frame_alloc();
        pFrameYUV = av_frame_alloc();
        auto *out_buffer = (uint8_t *) av_malloc(
                avpicture_get_size(AV_PIX_FMT_YUV420P, pCodecCtx->width, pCodecCtx->height));
        avpicture_fill((AVPicture *) pFrameYUV, out_buffer, AV_PIX_FMT_YUV420P, pCodecCtx->width, pCodecCtx->height);
        int ret, got_picture;
        auto *packet = (AVPacket *) av_malloc(sizeof(AVPacket));
        FILE *fp_yuv = fopen("output.yuv", "wb");
        struct SwsContext *img_convert_ctx;

        AVPixelFormat pixFormat;
        switch (cam->streams[0]->codec->pix_fmt) {
            case AV_PIX_FMT_YUVJ420P :
                pixFormat = AV_PIX_FMT_YUV420P;
                break;
            case AV_PIX_FMT_YUVJ422P  :
                pixFormat = AV_PIX_FMT_YUV422P;
                break;
            case AV_PIX_FMT_YUVJ444P   :
                pixFormat = AV_PIX_FMT_YUV444P;
                break;
            case AV_PIX_FMT_YUVJ440P :
                pixFormat = AV_PIX_FMT_YUV440P;
                break;
            default:
                pixFormat = *cam->streams[0]->codec->codec->pix_fmts;
                break;
        }

        img_convert_ctx = sws_getContext(pCodecCtx->width, pCodecCtx->height, pixFormat, pCodecCtx->width,
                                         pCodecCtx->height, AV_PIX_FMT_YUV420P, SWS_BICUBIC, nullptr, nullptr, nullptr);
        ///这里打印出视频的宽高
        fprintf(stderr, "w= %d h= %d\n", pCodecCtx->width, pCodecCtx->height);
        ///我们就读取100张图像
        for (int i = 0; i < 5; i++) {
            if (av_read_frame(cam, packet) < 0) {
                break;
            }
            if (packet->stream_index == videoindex) {
                ret = avcodec_decode_video2(pCodecCtx, pFrame, &got_picture, packet);
                if (ret < 0) {
                    printf("Decode Error.\n");
                    goto label_END;
                }
                if (got_picture) {
                    sws_scale(img_convert_ctx, (const uint8_t *const *) pFrame->data, pFrame->linesize, 0,
                              pCodecCtx->height, pFrameYUV->data, pFrameYUV->linesize);
                    int y_size = pCodecCtx->width * pCodecCtx->height;
                    InitPlay(1280,720);
                    memcpy(yuvBuffer, pFrameYUV->data[0], y_size);
                    memcpy(yuvBuffer + y_size, pFrameYUV->data[1], y_size/4);
                    memcpy(yuvBuffer + y_size+(y_size/4), pFrameYUV->data[2], y_size/4);
                    wb_yuv(yuvBuffer,y_size+(y_size/2));

                    QPainter p(this);
                    QPixmap tempPixmap = QPixmap::fromImage(image);
                    p.drawPixmap(450,0,tempPixmap);
                    ui->label->setPixmap(tempPixmap);
//                    fwrite(pFrameYUV->data[0], 1, y_size, fp_yuv);    //Y
//                    fwrite(pFrameYUV->data[1], 1, y_size / 4, fp_yuv);  //U
//                    fwrite(pFrameYUV->data[2], 1, y_size / 4, fp_yuv);  //V
                }
            }
            av_free_packet(packet);
        }
        sws_freeContext(img_convert_ctx);
        fclose(fp_yuv);
        av_free(out_buffer);
        av_free(pFrameYUV);
        avcodec_close(pCodecCtx);
    }
    label_END:;
    avformat_close_input(&cam);

}

Widget::~Widget() {
    delete ui;
}



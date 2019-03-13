/**
*  @file        win_obs_video_encoder.h
*  @brief       OBS�����������ļ�.
*  @author      sonysuqin
*  @copyright   sonysuqin
*  @version     1.0.0.1
*/

#ifdef OBS_ENCODER
#ifndef __WIN_OBS_VIDEO_ENCODER_H__
#define __WIN_OBS_VIDEO_ENCODER_H__

#include "libobs/obs.hpp"
#include "libobs/util/util.hpp"

extern "C" {
#include "libobs/obs-internal.h"
}

#include "api/video_codecs/video_encoder.h"
#include "rtc_base/asyncinvoker.h"
#include "media/engine/webrtcvideoencoderfactory.h"
#include "common_video/h264/h264_bitstream_parser.h"
#include "system_wrappers/include/clock.h"

namespace webrtc {

const size_t kFrameDiffThresholdMs  = 350;      //!< �����ݵ�������ʱ����ֵ.
const size_t kMinKeyFrameInterval   = 6;        //!< ��С�ؼ�֡�����ֵ.

/// OBS��������
class WinObsVideoEncoder : public webrtc::VideoEncoder {
public:
    /**
    *  @brief  WinObsVideoEncoder�๹�캯��.
    */
    WinObsVideoEncoder();

    /**
    *  @brief  WinObsVideoEncoder����������.
    */ 
    virtual ~WinObsVideoEncoder();

public:
    /**
    *  @brief  WebRTC��ʼ���������ص�.
    *  @param  codec_settings       ���������ã�Ŀǰ���ã���Ҫ�Թ��캯�������OBS����Ϊ׼.
    *  @param  number_of_cores      ���ú�������Ŀǰ����.
    *  @param  max_payload_size     ����غɴ�С��Ŀǰ����.
    *  @return ������.
    */
    virtual int32_t InitEncode(
        const webrtc::VideoCodec* codec_settings,
        int32_t number_of_cores,
        size_t max_payload_size) override;

    /**
    *  @brief  WebRTCע�������ɻص��Ļص�.
    *  @param  callback             ������ɻص���������֡ͨ���ûص�����WebRTC.
    *  @return ������.
    */
    virtual int32_t RegisterEncodeCompleteCallback(
        webrtc::EncodedImageCallback* callback) override;

    /**
    *  @brief  WebRTC�ͷű������ص�.
    *  @return ������.
    */
    virtual int32_t Release() override;

    /**
    *  @brief  WebRTC����ص�.
    *  @param  frame                ������֡.
    *  @param  codec_specific_info  ������Ϣ��Ŀǰδ��.
    *  @param  frame_types          ֡����, �ؼ�֡/�ǹؼ�֡.
    *  @return ������.
    */
    virtual int32_t Encode(
        const webrtc::VideoFrame& frame,
        const webrtc::CodecSpecificInfo* codec_specific_info,
        const std::vector<webrtc::FrameType>* frame_types) override;

    /**
    *  @brief  WebRTC֪ͨͨ�������ص�.
    *  @param  packet_loss          ������.
    *  @param  rtt                  rtt.
    *  @note   ������Ӧ�ø������������������ñ�������Ŀǰδ��.
    *  @return ������.
    */
    virtual int32_t SetChannelParameters(
        uint32_t packet_loss,
        int64_t rtt) override;

    /**
    *  @brief  WebRTC֪ͨ���ʡ�֡�ʻص�.
    *  @param  rate_allocation      ����.
    *  @param  frame_rate           ֡��.
    *  @note   ������Ӧ�ø������������������ñ�������Ŀǰδ��.
    *  @return ������.
    */
    virtual int32_t SetRateAllocation(
        const webrtc::BitrateAllocation& rate_allocation,
        uint32_t frame_rate) override;

private:
    /**
    *  @brief  ��OBS������.
    *  @return �ɹ�/ʧ��.
    */
    bool open();

    /**
    *  @brief  �ر�OBS������.
    *  @return �ɹ�/ʧ��.
    */
    bool close();

    /**
    *  @brief  ����OBS������.
    *  @return �ɹ�/ʧ��.
    */
    bool start();

    /**
    *  @brief  ֹͣOBS������.
    *  @return �ɹ�/ʧ��.
    */
    bool stop();

    /**
    *  @brief  �ڲ�����OBS������.
    *  @return �ɹ�/ʧ��.
    */
    bool start_internal();

    /**
    *  @brief  �ڲ�ֹͣOBS������.
    *  @return �ɹ�/ʧ��.
    */
    bool stop_internal();

    /**
    *  @brief  ���ñ�����.
    *  @param  config               �����ļ�.
    */
    void configure_encoder(const ConfigFile& config);

    /**
    *  @brief  OBS������ɻص�.
    *  @param  param                ����OBS�������Ļش�����(this).
    *  @param  packet               ������ɵİ�.
    */
    static void new_encoded_packet(void *param, struct encoder_packet *packet);

    /**
    *  @brief  ����OBS������ɵİ�.
    *  @param  packet               ������ɵİ�.
    */
    void on_new_encoded_packet(struct encoder_packet *packet);

    /**
    *  @brief  ����ָ����H264������.
    *  @param  encoderId            ����������.
    *  @return �ɹ�/ʧ��.
    */
    bool load_streaming_preset_h264(const char *encoderId);

    /**
    *  @brief  ����amd��������Ĭ������.
    *  @param  settings             H264���ö���.
    *  @param  bitrate              Ŀ������.
    */
    void update_streaming_settings_amd(obs_data_t *settings, int bitrate);

    /**
    *  @brief  ��ȡ�������Ļص���������.
    *  @param  encoder              OBS����������.
    *  @param  new_packet           ������ɻص�.
    *  @param  param                ����OBS�������Ļش�����(this).
    *  @return �ص���������.
    */
    size_t get_callback_idx(
        const struct obs_encoder *encoder,
        void(*new_packet)(void *param, struct encoder_packet *packet),
        void *param);

    /**
    *  @brief  ʵ���ͷ�OBS������.
    *  @param  encoder              OBS����������.
    */
    void obs_encoder_actually_destroy(obs_encoder_t *encoder);

    /**
    *  @brief  �ڱ����̳߳�ʼ��������.
    *  @param  width                ͼ���.
    *  @param  height               ͼ���.
    *  @param  target_bitrate       Ŀ������, Ŀǰ����.
    *  @param  fps                  ֡�ʣ� Ŀǰ����.
    *  @return ������.
    */
    int32_t init_encode_on_codec_thread(int32_t width, int32_t height, int32_t target_bitrate, int32_t fps);

    /**
    *  @brief  �ڱ����߳�����WebRTC������ɻص�.
    *  @param  callback             WebRTC������ɻص�.
    *  @return ������.
    */
    int32_t register_encode_complete_callback_on_codec_thread(webrtc::EncodedImageCallback* callback);

    /**
    *  @brief  �ڱ����߳��ͷű�����.
    *  @return ������.
    */
    int32_t release_on_codec_thread();

    /**
    *  @brief  �ڱ����̵߳��ñ���.
    *  @param  frame                ������֡.
    *  @param  frame_types          ֡����, �ؼ�֡/�ǹؼ�֡.
    *  @return ������.
    */
    int32_t encode_on_codec_thread(
        const webrtc::VideoFrame& frame,
        const webrtc::FrameType frame_type);

    /**
    *  @brief  OBS���뷽��.
    *  @param  frame                ������֡.
    *  @param  request_key_frame    �Ƿ��������ؼ�֡.
    *  @param  timestamp            RTPʱ�������1/90msΪ��λ.
    *  @param  render_time_ms       ��ʾʱ���.
    *  @return ������.
    */
    int32_t do_encode(
        struct encoder_frame *frame, 
        bool request_key_frame, 
        int32_t timestamp, 
        int64_t render_time_ms);

    /**
    *  @brief  ������ɵ��ûص����ͱ�����.
    *  @param  cb                   OBS������ɻص�.
    *  @param  packet               �ѱ�����ɵ�֡.
    *  @note   �����ڷ���ǰ������Ĺ������������SPS��PPS.
    */
    void send_packet(struct encoder_callback *cb, struct encoder_packet *packet);

    /**
    *  @brief  ���͵�һ����.
    *  @param  cb                   OBS������ɻص�.
    *  @param  packet               �ѱ�����ɵ�֡.
    */
    void send_first_video_packet(
        struct encoder_callback *cb,
        struct encoder_packet *packet);

    /**
    *  @brief  ����IDR֡����ʵ��WebRTCҪ��ÿ���ؼ�֡����IDR֡.
    *  @param  cb                   OBS������ɻص�.
    *  @param  packet               �ѱ�����ɵ�֡.
    */
    void send_idr_packet(
        struct encoder_callback *cb,
        struct encoder_packet *packet);

    /**
    *  @brief  ��ȡSEI��.
    *  @param  sei                  SEI������.
    *  @param  size                 ���峤��.
    */
    bool get_sei(
        uint8_t **sei,
        size_t *size);

private:
    /// ����֡��Ϣ�����ڴ洢ʱ�����Ϣ.
    struct InputFrameInfo {
        InputFrameInfo(
            int32_t frame_timestamp,
            int64_t frame_render_time_ms)
            : frame_timestamp(frame_timestamp),
              frame_render_time_ms(frame_render_time_ms) {}
        const int32_t frame_timestamp;
        const int64_t frame_render_time_ms;
    };

private:
    /// OBS����������.
    OBSEncoder h264_encoder_;
    /// �������Ƿ��Ѵ�.
    bool opened  = false;
    /// �������Ƿ�������.
    bool started = false;
    /// ����ͼ���.
    int32_t width_ = 0;
    /// ����ͼ���.
    int32_t height_ = 0;

    /// �����߳�.
    std::unique_ptr<rtc::Thread> encode_thread_;
    /// WebRTC������ɻص�.
    webrtc::EncodedImageCallback* callback_ = NULL;
    /// WebRTC H264������������Ҫ���ڻ�ȡQP.
    webrtc::H264BitstreamParser h264_bitstream_parser_;

    /// ����֡��ʱ�����Ϣ����.
    std::list<InputFrameInfo> input_frame_infos_;
    /// RTPʱ���, ��1/90msΪ��λ.
    int32_t output_timestamp_ = 0;
    /// ���ʱ�������msΪ��λ.
    int64_t output_render_time_ms_ = 0;

    /// ʱ��.
    webrtc::Clock* clock_;
    /// NTPʱ��(1900)��UNIXʱ��(1970)�Ĳ�ֵ.
    int64_t delta_ntp_internal_ms_ = 0;

    /// ����յ�������֡��ʱ��.
    int64_t last_frame_received_ms_ = 0;
    /// ���ϴ��յ�����ؼ�֡�����ʱ��.
    int32_t frames_received_since_last_key_ = 0;
    /// �Ƿ��һ֡.
    bool first_frame_ = true;

    /// ����������ͼ���ʽ.
    enum video_format preferred_video_format_ = VIDEO_FORMAT_NV12;

    /// NV12����.
    std::unique_ptr<uint8_t[]> nv12_buff_;
    /// NV12���峤��.
    size_t  frame_length_ = 0;
    /// NV12 Y Planeָ��.
    uint8_t *nv12_y_plane_ = NULL;
    /// NV12 UV Planeָ��.
    uint8_t *nv12_uv_plane_ = NULL;
};

////////////////////////////////////WinObsVideoEncoderFactory//////////////////////////////////////
class WinObsVideoEncoderFactory : public cricket::WebRtcVideoEncoderFactory {
public:
    WinObsVideoEncoderFactory();
    ~WinObsVideoEncoderFactory();

public:
    /**
    *  @brief  ����WebRTC��Ƶ������.
    *  @param  codec                ��������Ϣ.
    *  @return WebRTC��Ƶ������.
    */
    webrtc::VideoEncoder* CreateVideoEncoder(const cricket::VideoCodec& codec) override;

    /**
    *  @brief  ��ȡ֧�ֵı���.
    *  @return ������֧�ֵı���.
    */
    const std::vector<cricket::VideoCodec>& supported_codecs() const override;

    /**
    *  @brief  ����WebRTC��Ƶ������.
    *  @param  encoder              WebRTC��Ƶ������.
    */
    void DestroyVideoEncoder(webrtc::VideoEncoder* encoder) override;

private:
    /// ������֧�ֵı���.
    std::vector<cricket::VideoCodec> supported_codecs_;
};

} // namespace bee

#endif // #ifndef __WIN_OBS_VIDEO_ENCODER_H__
#endif // #ifdef OBS_ENCODER

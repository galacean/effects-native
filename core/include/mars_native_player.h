#ifndef MN_MARS_PLAYER_H_
#define MN_MARS_PLAYER_H_

#include <string>
#include <map>

#define DEFAULT_REPEAT_COUNT (-100)

#ifdef __GNUC__// it means the compiler is GCC version 4.0 or later
#ifdef ANDROID
#define MN_API_EXPORT __attribute__((visibility ("default")))
#define MN_API_LOCAL __attribute__((visibility("hidden")))
#else
#define MN_API_EXPORT
#define MN_API_LOCAL
#endif

#else
#error "##### requires gcc version >= 4.0 #####"
#endif

namespace mn {

/**
 * @brief MarsNative插件基类
 */
class MarsNativePlugin {
public:
    MN_API_EXPORT virtual ~MarsNativePlugin() {}

    /**
     * @brief MarsNativePlugin销毁通知，收到这个回调表示这个插件将不再使用
     * @return bool 返回true由marsnative销毁，否则由上层业务销毁
     */
    MN_API_EXPORT virtual bool OnDestroy() = 0;
};

/**
 * @brief MarsNative插件构造器
 */
class MarsNativePluginBuilder {
public:
    MN_API_EXPORT virtual MarsNativePlugin* CreateNew() = 0;
};

/**
 * @brief 图片解析器基类
 */
class MarsNativeImageDecoder {
public:
    virtual ~MarsNativeImageDecoder() {};

    /**
     * @brief 解析png/jpeg图片
     * @param out_width 返回图片宽度
     * @param out_height 返回图片高度
     * @param out_premultiply_alpha 返回是否预乘了alpha
     * @param data 原始数据
     * @param data_length 原始数据长度
     * @return uint8_t* 像素buffer，格式固定为RGBA，由MarsPlayer内部进行释放；解析失败则返回nullptr
     */
    virtual uint8_t* Decode(int& out_width, int& out_height, bool& out_premultiply_alpha, void* data, int data_length) = 0;
};

/**
 * @brief MarsNative图片数据，解码后的png/jpeg或压缩纹理原始数据
 */
struct MarsNativeImageData {
    /**
     * @brief MarsNativeImageData构造函数
     * @param url 图片原始地址
     * @param buffer 图片数据
     * @param need_free 是否需要由MarsNativeImageData销毁buffer
     */
    MarsNativeImageData(const std::string& url, void* buffer, bool need_free) {
        this->url = url;
        this->buffer = buffer;
        this->need_free = need_free;
    }

    ~MarsNativeImageData() {
        if (need_free) {
            free(buffer);
        }
    }

    std::string url;
    void* buffer = nullptr;

    // 压缩纹理
    int buffer_length = 0;

    // 图片解码后的数据，格式一定要是RGBA
    int width = 0; // 图片宽度
    int height = 0; // 图片高度
    bool premultiply_alpha = true; // 是否预乘过alpha

    bool need_free = false;
};

/**
 * @brief 创建MarsPlayer的参数
 * shared_egl_content: 共享上下文
 * width,height: 生成的GLTexture的大小
 * scene_data_buffer: mars.bin的内容，由MarsNativePlayer内部释放
 * repeat_count: 重复播放次数，-1为一直播放，默认按动画资源设置处理
 * plugins: 需要加载的插件
 */
struct MarsPlayerOptions {
    void* shared_egl_content = nullptr;
    int width = 1;
    int height = 1;
    void* scene_data_buffer = nullptr;
    // optional
    int repeat_count = DEFAULT_REPEAT_COUNT;
    std::map<std::string, MarsNativePluginBuilder*> plugins;
};

/**
 * @brief 解析通过mars-json-encoder binary打包出来的bin文件
 */
class MarsNativeData {
public:
    /**
     * @brief 创建一个MarsNativeData
     * @param buffer bin文件内容
     * @param length buffer的长度
     * @param need_free 是否需要由MarsNativeData销毁buffer
     * @param image_decoder 图片解析器；如果传空，使用内置图片解析器
     * @return MarsNativeData* MarsNativeData实例
     */
    MN_API_EXPORT static MarsNativeData* CreateNew(void* buffer, int length, bool need_free, MarsNativeImageDecoder* image_decoder = nullptr);

    virtual ~MarsNativeData() {}

    /**
     * @brief 返回动画设计宽度
     * @return int 
     */
    virtual int GetWidth() const = 0;

    /**
     * @brief 返回动画设计高度
     * @return int 
     */
    virtual int GetHeight() const = 0;

    /**
     * @brief 返回scene_data_buffer，直接赋值给MarsPlayerOptions。方法内部会新建一块buffer
     * @return void* 
     */
    virtual void* GetSceneData() const = 0;

    /**
     * @brief 返回图片数据数量
     * @return int 数量
     */
    virtual int GetImageDataCount() const = 0;

    /**
     * @brief 返回图片数据，不需要在外部释放
     * @param idx 图片索引
     * @return MarsNativeImageData* 图片数据，非法idx返回nullptr
     */
    virtual MarsNativeImageData* GetImageDataAt(int idx) const = 0;
    
    virtual int GetPluginCount() const = 0;
    
    virtual std::string GetPluginAt(int idx) const = 0;
};

enum class MarsPlayerResult {
    SUCCESS = 1,
    ERROR,
    NOT_INIT,
    ANIMATION_FINISH,
};

/**
 * C++版本MarsPlayer
 * 目前只支持离屏渲染的Player
 * 注意：Android上必须先在注册Logger
 * - 钱包环境通过AlipayLogger.create()注册
 * - 非钱包环境通过LogUtil.setLogger()注册自定义的logger
 */
class MarsNativePlayer {
public:
    MN_API_EXPORT static MarsNativePlayer* CreateNew(const MarsPlayerOptions& options, std::string* err = nullptr);

    MN_API_EXPORT void Destroy();

    MN_API_EXPORT bool Start();

    MN_API_EXPORT MarsPlayerResult Tick(int ms);

    MN_API_EXPORT bool Pause();

    MN_API_EXPORT bool Resume();

    MN_API_EXPORT bool SetImageData(MarsNativeImageData* image_data);

    MN_API_EXPORT uint32_t GetGLTexture();

private:
    ~MarsNativePlayer();

private:
    void* impl_ = nullptr;

    void* scene_data_ = nullptr;

    void* event_handler_ = nullptr;

    bool started_ = false;

    uint32_t texture_id_ = 0;
};

}

#endif

#include <Platform.h>

enum class WindowMode {
  Windowed = 0,
  Fullscreen,
  Borderless
};

typedef struct 
{
  int nWidth;
  int nHeight;
  WindowMode windowMode;
  bool bVsync;
} RENDERER_SETTINGS;

namespace Renderer
{
  extern const char * defaultShaderFilename;
  extern const char defaultShader[65536];

  extern int nWidth;
  extern int nHeight;

  bool OpenSetupDialog( RENDERER_SETTINGS * settings );
  bool Open( RENDERER_SETTINGS * settings );
  
  void StartFrame();
  void EndFrame();
  bool WantsToQuit();

  void RenderFullscreenQuad();

  bool ReloadShader( const char * szShaderCode, int nShaderCodeSize, char * szErrorBuffer, int nErrorBufferSize );
  void SetShaderConstant( const char * szConstName, float x );
  void SetShaderConstant( const char * szConstName, float x, float y );

  void StartTextRendering();
  void SetTextRenderingViewport( Scintilla::PRectangle rect );
  void EndTextRendering();

  bool GrabFrame( void * pPixelBuffer ); // input buffer must be able to hold w * h * 4 bytes of 0xAABBGGRR data

  void Close();

  enum class TextureType
  {
    Tex1D = 1,
    Tex2D = 2,
  };

  struct Texture
  {
    int width;
    int height;
    TextureType type;
  };

  Texture * CreateRGBA8Texture();
  Texture * CreateRGBA8TextureFromFile( const char * szFilename );
  Texture * CreateA8TextureFromData( int w, int h, const unsigned char * data );
  Texture * Create1DR32Texture( int w );
  bool UpdateR32Texture( Texture * tex, float * data );
  void SetShaderTexture( const char * szTextureName, Texture * tex );
  void BindTexture( Texture * tex ); // temporary function until all the quad rendering is moved to the renderer
  void ReleaseTexture( Texture * tex );

  void CopyBackbufferToTexture( Texture * tex );

  struct Vertex
  {
    Vertex( float _x, float _y, unsigned int _c = 0xFFFFFFFF, float _u = 0.0, float _v = 0.0) : 
      x(_x), y(_y), c(_c), u(_u), v(_v) {}
    float x, y;
    unsigned int c;
    float u, v;
  };
  void RenderQuad( const Vertex & a, const Vertex & b, const Vertex & c, const Vertex & d );
  void RenderLine( const Vertex & a, const Vertex & b );

  struct KeyEvent
  {
    int character;
    int scanCode;
    bool ctrl;
    bool shift;
    bool alt;
  };
  extern KeyEvent keyEventBuffer[512];
  extern int keyEventBufferCount;

  enum class MouseEventType
  {
    Down = 0,
    Move,
    Up,
    Scroll
  };
  enum class MouseButton
  {
    Left = 0,
    Right,
    Middle,
  };
  struct MouseEvent
  {
    MouseEventType eventType;
    float x;
    float y;
    MouseButton button;
  };
  extern MouseEvent mouseEventBuffer[512];
  extern int mouseEventBufferCount;
}

/*
 * MusicMouse on DPF
 * Copyright (C) 2026 Filipe Coelho <falktx@falktx.com>
 * SPDX-License-Identifier: ISC
 */

#include "DistrhoUI.hpp"
#include "Window.hpp"
#include "extra/WebView.hpp"

START_NAMESPACE_DISTRHO

// --------------------------------------------------------------------------------------------------------------------

class MusicMouseUI : public UI
{
    WebViewHandle webview = nullptr;

    // TESTING
    int idleCycle = 0;

public:
   /**
      UI class constructor.
      The UI should be initialized to a default state that matches the plugin side.
    */
    MusicMouseUI()
        : UI()
    {
        const double scaleFactor = getScaleFactor();
        setGeometryConstraints(DISTRHO_UI_DEFAULT_WIDTH * scaleFactor, DISTRHO_UI_DEFAULT_HEIGHT * scaleFactor);

        WebViewOptions opts;
        opts.initialJS = R"(
/* TODO this is incomplete!
// make sure we do not use audio in web context
class DummyAudioContext {
    constructor() {
        this.sampleRate = 48000;
        this.createBuffer = () => {
            return new DummyAudioBuffer(this);
        };
        this.createBufferSource = () => {
            return new DummyAudioBufferSourceNode(this);
        };
        this.createGain = () => {
            return new DummyAudioNode(this);
        };
    }
};
class DummyAudioBuffer {
    constructor(context) {
        this.context = context;
        this.numberOfChannels = 0;
        this.getChannelData = () => {
            return [];
        }
    }
};
class DummyAudioNode {
    constructor(context) {
        this.context = context;
        this.numberOfInputs = 0;
        this.numberOfOutputs = 0;
        this.gain = {
            cancelScheduledValues: () => { },
        };
    }
};
class DummyAudioBufferSourceNode {
    constructor(context) {
        this.context = context;
        this.numberOfInputs = 0;
        this.numberOfOutputs = 0;
        this.start = () => { };
        this.stop = () => { };
    }
};
window.AudioContext = DummyAudioContext;
*/
// TODO override WebMIDI and inject dpf postMessage()
// this is just a test called from C++ code
function sendTestMIDI() {
    setTimeout(function() {
        postMessage("oh hey I am a MIDI message, I swear!");
    }, 100);
};
)";
        opts.callback = _webviewMessageCallback;
        opts.callbackPtr = this;

        webview = webViewCreate(DISTRHO_PLUGIN_URI,
                                getWindow().getNativeWindowHandle(),
                                getWidth(),
                                getHeight(),
                                scaleFactor,
                                opts);
    }

    ~MusicMouseUI() override
    {
        if (webview != nullptr)
            webViewDestroy(webview);
    }

protected:
    // ----------------------------------------------------------------------------------------------------------------
    // DSP/Plugin Callbacks

   /**
      A parameter has changed on the plugin side.@n
      This is called by the host to inform the UI about parameter changes.
    */
    void parameterChanged(uint32_t index, float value) override
    {
    }

    void uiIdle()
    {
        if (webview == nullptr)
            return;

        webViewIdle(webview);

        // TESTING
        if (++idleCycle == 100)
        {
            idleCycle = 0;
            webViewEvaluateJS(webview, "sendTestMIDI()");
        }
    }

private:
    void webviewMessageCallback(char* msg)
    {
        // TODO receive MIDI through fake WebMIDI here
        d_stderr(msg);
    }

    static void _webviewMessageCallback(void* ptr, char* msg)
    {
        static_cast<MusicMouseUI*>(ptr)->webviewMessageCallback(msg);
    }

    DISTRHO_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MusicMouseUI)
};

// --------------------------------------------------------------------------------------------------------------------

UI* createUI()
{
    return new MusicMouseUI();
}

// --------------------------------------------------------------------------------------------------------------------

END_NAMESPACE_DISTRHO

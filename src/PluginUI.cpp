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

public:
   /**
      UI class constructor.
      The UI should be initialized to a default state that matches the plugin side.
    */
    MusicMouseUI()
        : UI()
    {
        const double scaleFactor = getScaleFactor();
        setGeometryConstraints((DISTRHO_UI_DEFAULT_WIDTH - 200) * scaleFactor,
                               (DISTRHO_UI_DEFAULT_HEIGHT - 200) * scaleFactor);

        WebViewOptions opts;
        opts.initialJS = R"(
const dpfMidiAccess = {
    // expose no inputs
    inputs: new Map(),

    // expose 1 output for sending MIDI through DPF
    outputs: new Map([
        ['dpf', {
            id: 'dpf',
            name: 'DPF Host',
            manufacturer: '',
            version: '',
            type: 'output',
            state: 'connected',
            connection: 'open',
            onmidimessage: null,
            onstatechange: null,
            addEventListener: () => { },
            removeEventListener: () => { },
            dispatchEvent: () => { },
            open: () => { },
            close: () => { },
            clear: () => { },
            send: (data) => {
                postMessage(JSON.stringify(data));
            },
        }],
    ]),

    // report sysex as enabled
    sysexEnabled: true,
};

navigator.requestMIDIAccess = () => new Promise((success, reject) => {
    success(dpfMidiAccess);
});

)";
        opts.callback = [](void* ptr, char* msg){
            static_cast<MusicMouseUI*>(ptr)->webviewMessageCallback(msg);
        };
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

    void uiIdle() override
    {
        if (webview == nullptr)
            return;

        webViewIdle(webview);
    }

    void uiScaleFactorChanged(double scaleFactor) override
    {
        if (webview != nullptr)
            webViewResize(webview, getWidth(), getHeight(), scaleFactor);
    }

    void onResize(const ResizeEvent& ev) override
    {
        if (webview != nullptr)
            webViewResize(webview, ev.size.getWidth(), ev.size.getHeight(), getScaleFactor());

        UI::onResize(ev);
    }

private:
    void webviewMessageCallback(char* msg)
    {
        // TODO receive MIDI through fake WebMIDI here
        d_stderr("got web message: %s", msg);
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

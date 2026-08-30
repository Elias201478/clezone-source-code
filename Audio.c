#include <windows.h>
#include <mmsystem.h>
#pragma comment(lib, "winmm.lib")

typedef unsigned char (*BytebeatFunc)(unsigned int t);

static HWAVEOUT hWaveOut = NULL;
static WAVEHDR hdr;
static unsigned char* buffer = NULL;
static volatile BOOL playing = FALSE;
static unsigned int t_global = 0;
static int sample_rate_global = 8000;
static BytebeatFunc func_global = NULL;

#define NUM_BUFFERS 2

static WAVEHDR hdrs[NUM_BUFFERS];
static unsigned char* bufs[NUM_BUFFERS];
static int current = 0;

void CALLBACK WaveCallback(HWAVEOUT hwo, UINT msg, DWORD_PTR inst, DWORD_PTR param1, DWORD_PTR param2)
{
    if (!playing) return;

    if (msg == WOM_DONE)
    {
        WAVEHDR* h = (WAVEHDR*)param1;

        
        for (int i = 0; i < sample_rate_global; i++, t_global++)
            ((unsigned char*)h->lpData)[i] = func_global(t_global);

        waveOutWrite(hWaveOut, h, sizeof(WAVEHDR));
    }
}

void PlayBytebeat(BytebeatFunc func, int hz)
{
    if (playing) return;

    func_global = func;
    sample_rate_global = hz;
    playing = TRUE;
    t_global = 0;

    WAVEFORMATEX wfx = { 0 };
    wfx.wFormatTag = WAVE_FORMAT_PCM;
    wfx.nChannels = 1;
    wfx.nSamplesPerSec = hz;
    wfx.wBitsPerSample = 8;
    wfx.nBlockAlign = 1;
    wfx.nAvgBytesPerSec = hz;

    waveOutOpen(&hWaveOut, WAVE_MAPPER, &wfx,
        (DWORD_PTR)WaveCallback, 0, CALLBACK_FUNCTION);

 
    for (int i = 0; i < NUM_BUFFERS; i++)
    {
        bufs[i] = (unsigned char*)malloc(hz);

        hdrs[i].lpData = (LPSTR)bufs[i];
        hdrs[i].dwBufferLength = hz;
        hdrs[i].dwFlags = 0;

        
        for (int s = 0; s < hz; s++, t_global++)
            bufs[i][s] = func(t_global);

        waveOutPrepareHeader(hWaveOut, &hdrs[i], sizeof(WAVEHDR));
        waveOutWrite(hWaveOut, &hdrs[i], sizeof(WAVEHDR));
    }
}


void StopBytebeat()
{
    if (!playing) return;

    playing = FALSE;

    waveOutReset(hWaveOut);
    waveOutUnprepareHeader(hWaveOut, &hdr, sizeof(WAVEHDR));
    waveOutClose(hWaveOut);

    free(buffer);
    buffer = NULL;
    hWaveOut = NULL;
}

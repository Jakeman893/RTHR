//C++ implementation of a text console for debug messages using the
// class described here https://github.com/Microsoft/DirectXTK/wiki/TextConsole
// I do not presume any kind of ownership over this portion of code however minor
// changes may be made to make this fit my needs better.

#pragma once
#include "Common\DeviceResources.h"
#include "SpriteBatch.h"
#include "SpriteFont.h"
#include "pch.h"

using namespace DX;
using std::shared_ptr;
using std::unique_ptr;
using std::vector;

class TextConsole
{
public:
	TextConsole();
	TextConsole(shared_ptr<DeviceResources> deviceResources, const wchar_t* fontName);
	~TextConsole();

	void Render();

	void Clear();

	void Write(const wchar_t *str);
	void WriteLine(const wchar_t *str);
	void Format(const wchar_t *strFormat, ...);

	void SetWindow(const RECT& layout);

	void XM_CALLCONV SetForegroundColor(DirectX::FXMVECTOR color) { DirectX::XMStoreFloat4(&m_textColor, color); }

	void ReleaseDevice();
	void RestoreDevice(shared_ptr<DeviceResources> deviceResources, const wchar_t* fontName);

private:
	void ProcessString(const wchar_t* str);
	void IncrementLine();

	RECT										m_layout;
	DirectX::XMFLOAT4							m_textColor;

	unsigned int								m_columns;
	unsigned int								m_rows;
	unsigned int								m_currentColumn;
	unsigned int								m_currentLine;
	
	unique_ptr<wchar_t[]>						m_buffer;
	unique_ptr<wchar_t*[]>						m_lines;
	vector<wchar_t>								m_tempBuffer;

	unique_ptr<DirectX::SpriteBatch>			m_batch;
	unique_ptr<DirectX::SpriteFont>				m_font;
	unique_ptr<DeviceResources>					m_device;

	std::mutex									m_mutex;
};
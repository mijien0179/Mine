#pragma once
#include <d2d1.h>
#pragma comment(lib, "d2d1")

#include <dwrite.h>
#pragma comment(lib, "dwrite")

#include "tools.h"

namespace Mine {
	interface IDrawObject {
	protected:
		D2D1_POINT_2F pos;
		D2D1_SIZE_F size;

		D2D1_COLOR_F foregroundColor;
		D2D1_COLOR_F backgroundColor;
	protected:
		D2D1_RECT_F GetRect() const {
			return D2D1::RectF(pos.x, pos.y, pos.x + size.width, pos.y + size.height);
		}
	public:
		virtual void OnDraw(ID2D1HwndRenderTarget*& pRT, ID2D1SolidColorBrush*& pBrush) = 0;
		
		IDrawObject(D2D1_POINT_2F pos, D2D1_SIZE_F size, D2D1_COLOR_F foreground, D2D1_COLOR_F background)
			: pos{pos}, size{size}, foregroundColor{foreground}, backgroundColor{background}
		{

		}
		virtual ~IDrawObject() {}
	};

	interface IDrawObjectShape : public IDrawObject{
	public:
		typedef unsigned char TYPE_DRAW_OPTION;
		enum class DrawOption : TYPE_DRAW_OPTION {
			inner = 0b0001,		//foregroundColor
			border = 0b0010		//backgroundColor
		};

		friend bool operator& (DrawOption oper1, DrawOption oper2) {
			return static_cast<bool>(static_cast<TYPE_DRAW_OPTION>(oper1) & static_cast<TYPE_DRAW_OPTION>(oper2));
		}

		friend DrawOption operator| (DrawOption oper1, DrawOption oper2) {
			return static_cast<DrawOption>(static_cast<TYPE_DRAW_OPTION>(oper1) | static_cast<TYPE_DRAW_OPTION>(oper2));
		}

	protected:
		DrawOption option;

	public:
		IDrawObjectShape(D2D1_POINT_2F pos, D2D1_SIZE_F size, DrawOption option, D2D1_COLOR_F innerColor, D2D1_COLOR_F borderColor)
			: IDrawObject{pos, size, innerColor, borderColor}, option{option}
		{

		}
	};

	class DrawObjectRectangle : public IDrawObjectShape {
	public:
		virtual void OnDraw(ID2D1HwndRenderTarget*& pRT, ID2D1SolidColorBrush*& pBrush) {
			if (option & DrawOption::inner && foregroundColor.a) {
				pBrush->SetColor(foregroundColor);
				pRT->FillRectangle(GetRect(), pBrush);
			}

			if (option & DrawOption::border && backgroundColor.a) {
				pBrush->SetColor(backgroundColor);
				pRT->DrawRectangle(GetRect(), pBrush);
			}
		}

	public:
		DrawObjectRectangle(D2D1_POINT_2F pos, D2D1_SIZE_F size, DrawOption option, D2D1_COLOR_F innerColor, D2D1_COLOR_F borderColor)
			: IDrawObjectShape{pos, size, option, innerColor, borderColor}
		{

		}

	};

	class DrawObjectText : public IDrawObject{
		wchar_t* text;
		size_t length;
		IDWriteTextFormat* pWTextFormat;

	public:
		size_t GetLength() const { return length; }
		const wchar_t* GetText() const { return text; }

		virtual void OnDraw(ID2D1HwndRenderTarget*& pRT, ID2D1SolidColorBrush*& pBrush) {
			if (backgroundColor.a) {
				pBrush->SetColor(backgroundColor);
				pRT->FillRectangle(GetRect(), pBrush);
			}
			pBrush->SetColor(foregroundColor);
			pRT->DrawTextW(text, length, pWTextFormat, GetRect(), pBrush);

		}

	public:
		DrawObjectText(wchar_t* str, IDWriteTextFormat* pWTextFormat,
			D2D1_POINT_2F pos, D2D1_SIZE_F size, D2D1_COLOR_F textColor, D2D1_COLOR_F backgroundColor)
			: IDrawObject{pos, size, textColor, backgroundColor}, pWTextFormat{pWTextFormat}, length{}
		{
			while (str[length++]);
			text = new wchar_t[length] {};
			for (size_t i = 0; str[i]; ++i) text[i] = str[i];

		}

		virtual ~DrawObjectText() {
			SafeRelease(pWTextFormat);
			if (text) {
				delete[] text;
				text = nullptr;
			}
		}
	};

	class DrawSet {
		IDrawObject** list;
		size_t size;
		size_t capacity;
	public:
		size_t GetSize() const { return size; }
		void AddItem(IDrawObject* item) {
			IDrawObject** temp;
			if (size == capacity) {
				capacity *= 2;
				temp = new IDrawObject * [capacity];
				for (size_t i = 0; i < size; ++i) {
					temp[i] = list[i];
				}
				delete[] list;
				list = temp;
			}
			list[size++] = item;
		}
		IDrawObject* GetItem(size_t index) {
			return list[index];
		}

		IDrawObject* operator[](size_t index) { return GetItem(index); }
		DrawSet& operator<<(IDrawObject* item) {
			AddItem(item);
			return *this;
		}
	public:
		DrawSet(size_t capacity) : capacity{capacity}, size{}{
			list = new IDrawObject * [capacity] {};
		}
		~DrawSet() {
			for (size_t i = 0; i < size; ++i) {
				delete list[i];
			}
			delete[] list;
		}
	};
}


namespace Mine {

	class DrawTool {
		ID2D1Factory* pFactory;
		ID2D1HwndRenderTarget* pRT;
		ID2D1SolidColorBrush* pBrush;
		
		HWND hWnd;
	private:
		inline HWND GetHwnd() const;
	public:
		LRESULT OnSize();
		LRESULT OnPaint(DrawSet& obj);
	public:
		DrawTool(HWND hWnd);
		~DrawTool();
	};

	class TextTool {
		IDWriteFactory* pWFactory;
		
	public:
		IDWriteTextFormat* CreateTextFormat(const wchar_t* fontName, float fontSize);
	public:
		TextTool();
		~TextTool();
	};

}
// ==============================================================
//	This file is part of Glest Shared Library (www.glest.org)
//
//	Copyright (C) 2001-2008 Martio Figueroa
//
//	You can redistribute this code and/or modify it under 
//	the terms of the GNU General Public License as published 
//	by the Free Software Foundation; either version 2 of the 
//	License, or (at your option) any later version
// ==============================================================

#include "text_renderer_gl.h"

#include "opengl.h"
#include "font_gl.h"
//#include <stdlib.h>
#include "leak_dumper.h"

namespace Shared{ namespace Graphics{ namespace Gl{

// =====================================================
//	class TextRenderer2DGl
// =====================================================

TextRenderer2DGl::TextRenderer2DGl(){
	rendering= false;
}

void TextRenderer2DGl::begin(const Font2D *font){
	assert(!rendering);
	rendering= true;
	
	this->font= static_cast<const Font2DGl*>(font);
}

//// Convert a narrow string to a wide string//
//std::wstring widen(const std::string& str) {
//	// Make space for wide string
//	wchar_t* buffer = new wchar_t[str.size() + 1];
//	// convert ASCII to UNICODE
//	mbstowcs( buffer, str.c_str(), str.size() );
//	// NULL terminate it
//	buffer[str.size()] = 0;
//	// Clean memory and return it
//	std::wstring wstr = buffer;
//	delete [] buffer;
//	return wstr;
//
//}
//// Widen an individual character

void TextRenderer2DGl::render(const string &text, int x, int y, bool centered, Vec3f *color) {
	assert(rendering);
	
	assertGl();

	if(color != NULL) {
		glPushAttrib(GL_CURRENT_BIT);
		glColor3fv(color->ptr());
	}

	int line=0;
	int size= font->getSize();
    const unsigned char *utext= reinterpret_cast<const unsigned char*>(text.c_str());

	Vec2f rasterPos;
	const FontMetrics *metrics= font->getMetrics();
	if(centered){
		rasterPos.x= x-metrics->getTextWidth(text)/2.f;
		rasterPos.y= y+metrics->getHeight()/2.f;
	}
	else{
		rasterPos= Vec2f(static_cast<float>(x), static_cast<float>(y));
	}
	glRasterPos2f(rasterPos.x, rasterPos.y);

	if(Font::fontIsMultibyte == true) {
		//setlocale(LC_CTYPE, "en_ca.UTF-8");

		//wstring wText = widen(text);
		//glListBase(font->getHandle());
		//glCallLists(wText.length(), GL_UNSIGNED_SHORT, &wText[0]);

		//string utfText = text;
		//glListBase(font->getHandle());
		//glCallLists(utfText.length(), GL_UNSIGNED_SHORT, &utfText[0]);

		string utfText = text;
		glListBase(font->getHandle());
		glCallLists(text.length(), GL_UNSIGNED_SHORT, &utext[0]);

		//std::locale loc("");
		//wstring wText = widen(text);
		//std::string strBuffer(Text.size() * 4 + 1, 0);
		//std::use_facet<std::ctype<wchar_t> >(loc).narrow(&Text[0], &Text[0] + Text.size(), '?', &strBuffer[0]);
		//string utfText = std::string(&strBuffer[0]);
		//glListBase(font->getHandle());
		//glCallLists(utfText.length(), GL_UNSIGNED_SHORT, &utfText[0]);
	}
	else {
		for (int i=0; utext[i]!='\0'; ++i) {
			switch(utext[i]){
			case '\t':
				rasterPos= Vec2f((rasterPos.x/size+3.f)*size, y-(size+1.f)*line);
				glRasterPos2f(rasterPos.x, rasterPos.y);
				break;
			case '\n':
				line++;
				rasterPos= Vec2f(static_cast<float>(x), y-(metrics->getHeight()*2.f)*line);
				glRasterPos2f(rasterPos.x, rasterPos.y);
				break;
			default:
				glCallList(font->getHandle()+utext[i]);
			}
		}
	}

	if(color != NULL) {
		glPopAttrib();
	}
	assertGl();
}

void TextRenderer2DGl::end(){
	assert(rendering);
	rendering= false;
}

// =====================================================
//	class TextRenderer3DGl
// =====================================================

TextRenderer3DGl::TextRenderer3DGl(){
	rendering= false;
}

void TextRenderer3DGl::begin(const Font3D *font){
	assert(!rendering);
	rendering= true;
	
	this->font= static_cast<const Font3DGl*>(font);

	assertGl();

	//load color
	glPushAttrib(GL_TRANSFORM_BIT);

	assertGl();
}

void TextRenderer3DGl::render(const string &text, float  x, float y, float size, bool centered){
	assert(rendering);
	
	assertGl();

	const unsigned char *utext= reinterpret_cast<const unsigned char*>(text.c_str());

	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glPushAttrib(GL_POLYGON_BIT);
	float scale= size/10.f;
	if(centered){
		const FontMetrics *metrics= font->getMetrics();
		glTranslatef(x-scale*metrics->getTextWidth(text)/2.f, y-scale*metrics->getHeight()/2.f, 0);
	}
	else{
		glTranslatef(x-scale, y-scale, 0);
	}
	glScalef(scale, scale, scale);
                     
	for (int i=0; utext[i]!='\0'; ++i) {
		glCallList(font->getHandle()+utext[i]);
	}

	glPopMatrix();
	glPopAttrib();

	assertGl();
}

void TextRenderer3DGl::end(){
	assert(rendering);
	rendering= false;

	assertGl();

	glPopAttrib();

	assertGl();
}

}}}//end namespace

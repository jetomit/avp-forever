// Copyright (C) 2010 Barry Duncan. All Rights Reserved.
// The original author of this code can be contacted at: bduncan22@hotmail.com

// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:
//
// 1. Redistributions of source code must retain the above copyright notice,
//    this list of conditions and the following disclaimer.
//
// 2. Redistributions in binary form must reproduce the above copyright notice,
//    this list of conditions and the following disclaimer in the documentation
//    and/or other materials provided with the distribution.
//
// THIS SOFTWARE IS PROVIDED ``AS IS'' AND ANY EXPRESS OR IMPLIED WARRANTIES,
// INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND
// FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
// DEVELOPERS AND CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
// SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
// PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
// OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
// WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
// OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
// ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

#include "vertexBuffer.h"
#include "logString.h"

bool VertexBuffer::Draw()
{
	R_SetVertexBuffer(this->vertexBuffer, this->vbFVFsize);
	R_DrawPrimitive(2);

	return true;
}

bool VertexBuffer::Lock(void **data)
{
	R_LockVertexBuffer(this->vertexBuffer, 0, 0, data, this->vbUsage);

	return true;
}

bool VertexBuffer::Unlock()
{
	R_UnlockVertexBuffer(this->vertexBuffer);

	return true;
}

bool VertexBuffer::Create(uint32_t size, enum FVF fvf, enum R_USAGE usage)
{
	this->vbFVF = fvf;

	switch (this->vbFVF)
	{
		case FVF_LVERTEX:
			this->vbFVFsize = sizeof(D3DLVERTEX);
			break;
		case FVF_ORTHO:
			this->vbFVFsize = sizeof(ORTHOVERTEX);
			break;
		case FVF_FMV:
			this->vbFVFsize = sizeof(FMVVERTEX);
			break;
		default:
			// error and return
			break;
	}

	this->vbLength = size * this->vbFVFsize;

	R_CreateVertexBuffer(this->vbLength, this->vbUsage, &vertexBuffer);

	return true;
}
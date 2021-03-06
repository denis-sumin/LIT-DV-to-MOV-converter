// MVTools v2

// Copyright(c)2008 A.G.Balakhnin aka Fizick
// Prepare super clip (hierachical levels data) for MVAnalyse

// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA, or visit
// http://www.gnu.org/copyleft/gpl.html .

#ifndef __MV_SUPER__
#define __MV_SUPER__

#include "MVInterface.h"
#include "GroupOfPlanes.h"
#include "commonfunctions.h"
#include "yuy2planes.h"

//    vi.num_audio_samples = nHeight + (nHPad<<16) + (nVPad<<24) + ((_int64)(nPel)<<32) + ((_int64)nModeYUV<<40) + ((_int64)nLevels<<48);

inline int PlaneHeightLuma(int src_height, int level, int yRatioUV, int vpad)
    {
        int height = src_height;

            for (int i=1; i<=level; i++)
            {
//                height = (height/2) - ((height/2) % yRatioUV) ;
                height = vpad >= yRatioUV ? ((height/yRatioUV + 1) / 2) * yRatioUV : ((height/yRatioUV) / 2) * yRatioUV;
            }
        return height;
}

inline int PlaneWidthLuma(int src_width, int level, int xRatioUV, int hpad)
    {
        int width = src_width;

            for (int i=1; i<=level; i++)
            {
//                width = (width/2) - ((width/2) % xRatioUV) ;
                width = hpad >= xRatioUV ? ((width/xRatioUV + 1) / 2) * xRatioUV : ((width/xRatioUV) / 2) * xRatioUV;
            }
        return width;
}

inline unsigned int PlaneSuperOffset(bool chroma, int src_height, int level, int pel, int vpad, int plane_pitch, int yRatioUV)
    {
        // storing subplanes in superframes may be implemented by various ways
        int height = src_height; // luma or chroma

        unsigned int offset;

        if (level==0)
            offset = 0;
        else
        {
            offset = pel*pel*plane_pitch*(src_height + vpad*2);

            for (int i=1; i<level; i++)
            {
                height = chroma ? PlaneHeightLuma(src_height*yRatioUV, i, yRatioUV, vpad*yRatioUV)/yRatioUV : PlaneHeightLuma(src_height, i, yRatioUV, vpad);

                offset += plane_pitch*(height + vpad*2);
            }
        }
        return offset;
}


class MVSuper : public GenericVideoFilter {
protected:

    int nHPad;
    int nVPad;
    int nPel;
    int nLevels;
    int sharp;
	int rfilter; // frame reduce filter mode
	PClip pelclip; // upsized source clip with doubled frame width and heigth (used for pel=2)
	bool isse;
	bool planar; //v2.0.0.7

    int nWidth;
    int nHeight;

    int yRatioUV;
    int xRatioUV;
    bool chroma;
    int pixelType;
    bool usePelClip;
    int nSuperWidth;
    int nSuperHeight;

    MVPlaneSet nModeYUV;

	YUY2Planes * SrcPlanes;
//	YUY2Planes * DstPlanes;
	YUY2Planes * SrcPelPlanes;

    MVGroupOfFrames *pSrcGOF;
    bool isPelClipPadded;

public :

	MVSuper(PClip _child, int _hpad, int _vpad, int pel, int _levels, bool _chroma, int _sharp, int _rfilter, PClip _pelclip,
			 bool _isse, bool _planar, IScriptEnvironment* env);
	~MVSuper();

   PVideoFrame __stdcall GetFrame(int n, IScriptEnvironment* env);


};

#endif

**
** Quake for AMIGA
** r_bsp.c assembler implementations by Frank Wille <frank@phoenix.owl.de>
**

		IFD	NQ_HACK
		INCLUDE	"quakedef68k-nq.i"
		ELSE
		IFD	QW_HACK
		INCLUDE	"quakedef68k-qw.i"
		ENDIF
		ENDIF

		XREF    _r_visframecount
		XREF    _r_framecount
		XREF    _r_currentkey
		XREF    _r_drawpolys
		XREF    _r_worldpolysbacktofront
		XREF    _pfrustum_indexes
		XREF    _modelorg
		XREF    _vpn
		XREF    _vright
		XREF    _vup
		XREF    _cl
		XREF    _view_clipplanes
		XREF    _screenedge
		XREF    _entity_rotation

		XREF    _R_RenderFace

		XDEF    _R_RotateBmodel
		XDEF    _R_RecursiveWorldNode



CONTENTS_SOLID          =       -2              ;bspfile.h
BMODEL_FULLY_CLIPPED	=	$10		;r_local.h

PLANE_X                 =       0
PLANE_Y                 =       1
PLANE_Z                 =       2
SURF_PLANEBACK          =       2
BACKFACE_EPSILON        equ.s   0.01            ;r_local.h
MAX_BTOFPOLYS           =       5000
PITCH                   =       0
YAW                     =       1
ROLL                    =       2



******************************************************************************
*
*       void _R_RotateBmodel (const entity_t *e)
*
******************************************************************************

		cnop    0,4
_R_RotateBmodel
		movem.l a2/a3,-(sp)
		fmovem.x        fp2-fp7,-(sp)
		lea     -9*4(sp),sp
		move.l  sp,a2                   ;a2 -> temp2
		lea     _entity_rotation,a3
		movea.l 2*4+6*12+9*4+4(sp),a0	; a2/a3, fp2-fp7, 9*4, addr
		lea     sincostab,a1
		fmove.s #8,fp7
		fmove.s ENTITY_ANGLES+YAW*4(a0),fp0
		fmul    fp7,fp0
		fmove.l fp0,d0                  ;d0 = angle
		fmove.s 0(a1,d0.l*8),fp0
		fmove.s 4(a1,d0.l*8),fp1
		fmove.s ENTITY_ANGLES+PITCH*4(a0),fp2
		fmul    fp7,fp2
		fmove.l fp2,d0                  ;d0 = angle
		fmove.s 0(a1,d0.l*8),fp2
		fmove.s 4(a1,d0.l*8),fp3
		fmove   fp3,fp5
		fmul    fp1,fp5
		fmove.s fp5,(a3)
		fmove   fp3,fp4
		fmul    fp0,fp4
		fmove.s fp4,4(a2)
		fneg    fp2
		fmove.s fp2,8(a2)
		fneg    fp2
		fneg    fp0
		fmove.s fp0,12(a3)
		fneg    fp0
		fmove   fp1,fp5
		fmove   fp2,fp4
		fmul    fp1,fp4
		fmove.s fp4,24(a3)
		fmove   fp2,fp4
		fmul    fp0,fp4
		fmove.s fp4,28(a2)
		fmove.s fp3,32(a2)
		fmove.s ENTITY_ANGLES+ROLL*4(a0),fp6
		fmul    fp7,fp6
		fmove.l fp6,d0                  ;d0 = angle
		fmove.s 0(a1,d0.l*8),fp6
		fmove.s 4(a1,d0.l*8),fp7
		fmove.s 4(a2),fp0
		fmove.s 8(a2),fp1
		fmove   fp0,fp2
		fmul    fp7,fp2
		fmove   fp1,fp3
		fmul    fp6,fp3
		fsub    fp3,fp2
		fmove.s fp2,4(a3)
		fmul    fp6,fp0
		fmul    fp7,fp1
		fadd    fp1,fp0
		fmove.s fp0,8(a3)
		fmove   fp5,fp0
		fmul    fp7,fp0
		fmove.s fp0,16(a3)
		fmul    fp6,fp5
		fmove.s fp5,20(a3)
		fmove.s 28(a2),fp0
		fmove.s 32(a2),fp1
		fmove   fp0,fp2
		fmul    fp7,fp2
		fmove   fp1,fp3
		fmul    fp6,fp3
		fsub    fp3,fp2
		fmove.s fp2,28(a3)
		fmul    fp6,fp0
		fmul    fp7,fp1
		fadd    fp1,fp0
		fmove.s fp0,32(a3)

		fmove.s (a3)+,fp0
		fmove.s (a3)+,fp1
		fmove.s (a3)+,fp2

*        R_EntityRotate (modelorg);
*        R_EntityRotate (vpn);
*        R_EntityRotate (vright);
*        R_EntityRotate (vup);

		lea     _modelorg,a0
		bsr     EntityRotate
		lea     -(36-12)(a3),a3
		lea     _vpn,a0
		bsr     EntityRotate
		lea     -(36-12)(a3),a3
		lea     _vright,a0
		bsr     EntityRotate
		lea     -(36-12)(a3),a3
		lea     _vup,a0
		bsr     EntityRotate

****** R_TransformFrustum (inlined)

		moveq   #4-1,d0
		lea     _screenedge,a0
		lea     _view_clipplanes,a1
		lea     _modelorg,a3
		fmove.s (a3)+,fp7
.loop
		fmove.s (a0)+,fp1
		fneg    fp1
		fmove.s (a0)+,fp2
		fmove.s (a0)+,fp0
		lea     _vright,a2
		fmove.s (a2)+,fp3
		fmove.s (a2)+,fp4
		fmove.s (a2)+,fp5
		fmul    fp1,fp3
		fmul    fp1,fp4
		fmul    fp1,fp5
		lea     _vup,a2
		fmove.s (a2)+,fp6
		fmul    fp2,fp6
		fadd    fp6,fp3
		fmove.s (a2)+,fp6
		fmul    fp2,fp6
		fadd    fp6,fp4
		fmove.s (a2)+,fp6
		fmul    fp2,fp6
		fadd    fp6,fp5
		lea     _vpn,a2
		fmove.s (a2)+,fp6
		fmul    fp0,fp6
		fadd    fp6,fp3
		fmove.s fp3,(a1)+
		fmove.s (a2)+,fp6
		fmul    fp0,fp6
		fadd    fp6,fp4
		fmove.s fp4,(a1)+
		fmove.s (a2)+,fp6
		fmul    fp0,fp6
		fadd    fp6,fp5
		fmove.s fp5,(a1)+

		fmul    fp7,fp3
		fmul.s  (a3),fp4
		fadd    fp4,fp3
		fmul.s  4(a3),fp5
		fadd    fp5,fp3
		fmove.s fp3,(a1)+
		lea     CLIP_SIZEOF-16(a1),a1
		lea     MPLANE_SIZEOF-12(a0),a0
		dbra    d0,.loop

******  end of R_TransformFrustum

		lea     9*4(sp),sp
		fmovem.x        (sp)+,fp2-fp7
		movem.l (sp)+,a2/a3
		rts

******  EntityRotate (inlined)

EntityRotate
		fmove.s (a0)+,fp3
		fmove.s (a0)+,fp4
		fmove.s (a0)+,fp5
		lea     -12(a0),a0
		fmove   fp3,fp6
		fmul    fp0,fp6
		fmove   fp4,fp7
		fmul    fp1,fp7
		fadd    fp7,fp6
		fmove   fp5,fp7
		fmul    fp2,fp7
		fadd    fp7,fp6
		fmove.s fp6,(a0)+
		fmove.s (a3)+,fp6
		fmul    fp3,fp6
		fmove.s (a3)+,fp7
		fmul    fp4,fp7
		fadd    fp7,fp6
		fmove.s (a3)+,fp7
		fmul    fp5,fp7
		fadd    fp7,fp6
		fmove.s fp6,(a0)+
		fmove.s (a3)+,fp6
		fmul    fp3,fp6
		fmove.s (a3)+,fp7
		fmul    fp4,fp7
		fadd    fp7,fp6
		fmove.s (a3)+,fp7
		fmul    fp5,fp7
		fadd    fp7,fp6
		fmove.s fp6,(a0)+
		rts

******  end of EntityRotate






******************************************************************************
*
*       void _R_RecursiveWorldNode (const entity_t *e, mnode_t *node)
*
******************************************************************************

		cnop    0,4
_R_RecursiveWorldNode

		rsreset
		rs.l    1
.e		rs.l	1
.node           rs.l    1

		move.l  .node(sp),a0
		move.l  .e(sp),d0
		bsr     DoRecursion
		rts

DoRecursion
		movem.l d2-d4/a2/a3,-(sp)
		fmovem.x        fp2-fp3,-(sp)
		move.l  d0,d2
		move.l  a0,a2

*    if (node->contents == CONTENTS_SOLID)
*	return;
*    if (node->visframe != r_visframecount)
*	return;
*    if (node->clipflags == BMODEL_FULLY_CLIPPED)
*	return;

		cmp.l   #CONTENTS_SOLID,NODE_CONTENTS(a2)
		beq.w   .end
		move.l  NODE_VISFRAME(a2),d0
		cmp.l   _r_visframecount,d0
		bne.w   .end
		cmp.l	#BMODEL_FULLY_CLIPPED,NODE_CLIPFLAGS(a2)
		beq.w	.end

*    if (node->contents < 0)

		tst.l   NODE_CONTENTS(a2)
		bge.b   .else

*	pleaf = (mleaf_t *)node;
*	pleaf->key = r_currentkey;
*	r_currentkey++;		// all bmodels in a leaf share the same key
*
*	return;

		move.l  _r_currentkey,LEAF_KEY(a2)
		addq.l  #1,_r_currentkey        ;r_currentkey++
		bra.w   .end
		
*                plane = node->plane;
*
*                switch (plane->type)
*                {
*                case PLANE_X:
*                        dot = modelorg[0] - plane->dist;
*                        break;
*                case PLANE_Y:
*                        dot = modelorg[1] - plane->dist;
*                        break;
*                case PLANE_Z:
*                        dot = modelorg[2] - plane->dist;
*                        break;
*                default:
*                        dot = DotProduct (modelorg, plane->normal) - plane->dist;
*                        break;

.else
		move.l  NODE_PLANE(a2),a0       ;plane = node->plane
		lea     _modelorg,a1
		move.b  MPLANE_TYPE(a0),d0      ;switch (plane->type)
		fmove.s MPLANE_DIST(a0),fp0
		cmp.b   #PLANE_X,d0             ;case PLANE_X
		beq.b   .x
		cmp.b   #PLANE_Y,d0             ;case PLANE_Y
		beq.b   .y
		cmp.b   #PLANE_Z,d0             ;case PLANE_Z
		beq.b   .z
.def
		fmove.s (a1)+,fp1               ;dot = DotProduct(...)
		fmove.s (a1)+,fp2
		fmove.s (a1)+,fp3
		fmul.s  (a0)+,fp1
		fmul.s  (a0)+,fp2
		fmul.s  (a0)+,fp3
		fadd    fp1,fp2
		fadd    fp3,fp2
		fsub    fp0,fp2
		bra.b   .cont3
.x
		fmove.s (a1),fp2                ;dot = modelorg[0] - plane->dist
		fsub    fp0,fp2
		bra.b   .cont3
.y
		fmove.s 4(a1),fp2               ;dot = modelorg[1] - plane->dist
		fsub    fp0,fp2
		bra.b   .cont3
.z
		fmove.s 8(a1),fp2               ;dot = modelorg[2] - plane->dist
		fsub    fp0,fp2

*                if (dot >= 0)
*                        side = 0;
*                else
*                        side = 1;


.cont3
		ftst    fp2                     ;if (dot >= 0)
		fbolt.w .lt
		moveq   #0,d3                   ;side = 0
		bra.b   .ge
.lt
		moveq   #1,d3                   ;side = 1
.ge

*                R_RecursiveWorldNode(e, node->children[side]);

		move.l  d2,d0
		move.l  NODE_CHILDREN(a2,d3.l*4),a0
		bsr     DoRecursion

*                c = node->numsurfaces;
*
*                if (c)

		move.l  NODE_NUMSURFACES(a2),d4 ;c = node->numsurfaces
		subq.l  #1,d4                   ;if (c)
		bmi.b   .skip

*                        surf = cl.worldmodel->surfaces + node->firstsurface;
*
		move.l  _cl+CL_WORLDMODEL,a0    ;surf = cl.worldmodel + ...
		move.l  BRUSHMODEL_SURFACES(a0),a3
		move.l  NODE_FIRSTSURFACE(a2),d0
		asl.l   #MSURFACE_SIZEOF_EXP,d0
		add.l   d0,a3	; a3 = surf
		
*	for (count = node->numsurfaces; count; count--, surf++) {
*	    if (surf->visframe != r_visframecount)
*		continue;
*	    if (surf->clipflags == BMODEL_FULLY_CLIPPED)
*		continue;
*	    R_RenderFace(e, surf, surf->clipflags);
*	}

		
.loop3
		move.l	_r_visframecount,d0
		cmp.l	MSURFACE_VISFRAME(a3),d0
		bne.b	.next3
		cmp.l	#BMODEL_FULLY_CLIPPED,MSURFACE_CLIPFLAGS(a3)
		beq.b	.next3
		
		move.l  MSURFACE_CLIPFLAGS(a3),-(sp)
		move.l  a3,-(sp)
		move.l	d2,-(sp)
		jsr     _R_RenderFace
		lea    12(sp),sp
.next3
		lea     MSURFACE_SIZEOF(a3),a3
		dbra	d4,.loop3

*	r_currentkey++;

		addq.l  #1,_r_currentkey
.skip

*                R_RecursiveWorldNode (node->children[!side], clipflags);

		move.l  d2,d0
		eori.l  #1,d3
		move.l  NODE_CHILDREN(a2,d3.l*4),a0
		bsr     DoRecursion
.end
		fmovem.x        (sp)+,fp2-fp3
		movem.l (sp)+,d2-d4/a2/a3
		rts


sincostab       incbin  "sincos.bin"

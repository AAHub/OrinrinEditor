/*! @file
	@brief �t�@�C�������╶���񌟍��̏��������܂�
	���̃t�@�C���� DocSearchCtrl.cpp �ł��B
	@author	SikigamiHNQ
	@date	2011/11/15
*/

/*
Orinrin Editor : AsciiArt Story Editor for Japanese Only
Copyright (C) 2011 - 2013 Orinrin/SikigamiHNQ

This program is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
See the GNU General Public License for more details.
You should have received a copy of the GNU General Public License along with this program.
If not, see <http://www.gnu.org/licenses/>.
*/
//-------------------------------------------------------------------------------------------------

#include "stdafx.h"
#include "OrinrinEditor.h"
//-------------------------------------------------------------------------------------------------

/*
Ctrl+F�Ō������I�[�|��

Ctrl�{�e�R�ŁA�I��͈͂������͈͂ɁE�͈͂Ȃ������疳��
�e�R�Ŏ��̌���������̂���łփW�����v
�e�T�Ń��t���b�V�����ȈՑΉ�


����������A�t�@�C�����E�Ŕԍ��E�ʒu��S���o���Ă����K�v�����遃�s�v�H

�C�e���[�^�Aerase�̕Ԃ�l�́A�폜������̎��̈ʒu

F3�W�����v�̋L�^�A�ňړ�������A���̕łɍ��킹��
�V�K���������炻��ɍ��킹��

�����ʒu�ɕ�����ǉ��폜�����Ƃ��̏����́H
�땶���ǉ��폜�E���̍s����������x�T�[�`����΂���
���s�܂ł͂�������A�ĕ`��͈͒���

�y�[�X�g�Ƃ��A�I���폜�݂����ȑ�Z�͂�������
�폜��,�I�������̃J�[�\���ʒu�ł����邩�H�y�[�X�g�́A�n�߂��s��,�y�[�X�g�͈͍s�����ŁH

*/

#ifdef FIND_STRINGS



//	�q�b�g�ʒu���L�^
//typedef struct tagFINDPOSITION
//{
//	LPARAM	dUnique;	//!<	�t�@�C���ʂ��ԍ��E�P�C���f�b�N�X
//	INT		iPage;		//!<	�����Ă��
//	INT		iLine;		//!<	�Y���s
//	INT		iCaret;		//!<	�s���ł̕����ʒu
//
//} FINDPOSITION, *LPFINDPOSITION;

//-------------------------------------------------------------------------------------------------

extern list<ONEFILE>	gltMultiFiles;	//	�����t�@�C���ێ�
//�C�e���[�^��typedef�̓w�b�_��

extern FILES_ITR	gitFileIt;			//		�����Ă�t�@�C���̖{��

extern INT		gixFocusPage;			//		���ڒ��̃y�[�W�E�Ƃ肠�����O�E�O�C���f�b�N�X

EXTERNED HWND	ghFindDlg;				//!<	�����_�C�����O�̃n���h��


static TCHAR	gatNowPtn[MAX_PATH];	//!<	�ŐV�̌���������
static INT		gixFindMode;			//!<	�ŐV�̌������[�h�@�O�ł̂݁@�P�P�t�@�C���@�L�����Z�����Q�S�t�@�C��
static INT		giCrLfCnt;				//!<	���������񒆂ɉ��s���������邩


//static FINDPOSITION	gstFindPos;			//!<	�����W�����v�ʒu

//static list<FINDPOSITION>	gltFindPosition;	//!<	�������ʕێ�
//-------------------------------------------------------------------------------------------------


INT_PTR	CALLBACK FindStrDlgProc( HWND, UINT, WPARAM, LPARAM );	//!<	
HRESULT	FindExecute( HWND );	//!<	
HRESULT	FindPageSearch( LPTSTR, INT, FILES_ITR );	//!<	
INT		FindPageHighlightOff( INT , FILES_ITR );	//!<	
HRESULT	FindPageHighlightSet( INT, INT, INT, FILES_ITR );	//!<	
HRESULT FindLineHighlightOff( UINT , LINE_ITR );	//!<	
//-------------------------------------------------------------------------------------------------


/*!
	�����_�C�����O���J���E���[�_���X��
	@param[in]	hInst	�A�|���P�[�V�����̎���
	@param[in]	hWnd	�E�C���h�E�n���h��
	@retval HRESULT	�I����ԃR�[�h
*/
HRESULT FindDialogueOpen( HINSTANCE hInst, HWND hWnd )
{

	if( !(hInst) || !(hWnd) )	//	�ϐ����������Ă�������
	{
		ZeroMemory( gatNowPtn, sizeof(gatNowPtn) );
		gixFindMode = 0;
		giCrLfCnt = 0;
	}

	if( ghFindDlg )
	{
		SetForegroundWindow( ghFindDlg );
		return S_FALSE;
	}

	ghFindDlg = CreateDialogParam( hInst, MAKEINTRESOURCE(IDD_FIND_STRING_DLG), hWnd, FindStrDlgProc, 0 );

	ShowWindow( ghFindDlg, SW_SHOW );

	return S_OK;
}
//-------------------------------------------------------------------------------------------------

/*!
	�����_�C�����O�̃v���V�[�W��
	@param[in]	hDlg	�_�C�����O�n���h��
	@param[in]	message	�E�C���h�E���b�Z�[�W�̎��ʔԍ�
	@param[in]	wParam	�ǉ��̏��P
	@param[in]	lParam	�ǉ��̏��Q
	@retval 0	���b�Z�[�W�͏������Ă��Ȃ�
	@retval no0	�Ȃ񂩏������ꂽ
*/
INT_PTR CALLBACK FindStrDlgProc( HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam )
{
	HWND	hWorkWnd;
	UINT	id;
//	HWND	hWndChild;


	switch( message )
	{
		default:	break;

		case WM_INITDIALOG:
			//	�R���{�{�b�N�X�ɍ��ړ����
			hWorkWnd = GetDlgItem( hDlg, IDCB_FIND_TARGET );
			ComboBox_InsertString( hWorkWnd, 0, TEXT("���݂̕�") );
			ComboBox_InsertString( hWorkWnd, 1, TEXT("���݂̃t�@�C��") );
	//		ComboBox_InsertString( hWorkWnd, 2, TEXT("�J���Ă���S�Ẵt�@�C��") );������
			ComboBox_SetCurSel( hWorkWnd, gixFindMode );	//	���̌������[�h�𔽉f����
	//		ComboBox_Enable( hWorkWnd , FALSE );	//	�@�\�o����܂œ���

			hWorkWnd = GetDlgItem( hDlg, IDE_FIND_TEXT );
			Edit_SetText( hWorkWnd, gatNowPtn );	//	���̌������e������Γ]�ʂ���
			SetFocus( hWorkWnd );
			return (INT_PTR)FALSE;


		case WM_COMMAND:
			id = LOWORD(wParam);
			hWorkWnd = GetDlgItem( hDlg, IDE_FIND_TEXT );
			switch( id )
			{
				case IDCANCEL:	DestroyWindow( hDlg );	return (INT_PTR)TRUE;
				case IDOK:		FindExecute( hDlg );	return (INT_PTR)TRUE;	//	��������

				case IDM_PASTE:	SendMessage( hWorkWnd, WM_PASTE, 0, 0 );	return (INT_PTR)TRUE;
				case IDM_COPY:	SendMessage( hWorkWnd, WM_COPY,  0, 0 );	return (INT_PTR)TRUE;
				case IDM_CUT:	SendMessage( hWorkWnd, WM_CUT,   0, 0 );	return (INT_PTR)TRUE;
				case IDM_UNDO:	SendMessage( hWorkWnd, WM_UNDO,  0, 0 );	return (INT_PTR)TRUE;

				case IDB_FIND_CLEAR:	FindHighlightOff(  );	return (INT_PTR)TRUE;

				default:	break;
			}
			break;


		case WM_CLOSE:
			DestroyWindow( hDlg );
			ghFindDlg = NULL;
			return (INT_PTR)TRUE;

		case WM_DESTROY:
			ViewFocusSet(  );	//	�t�H�[�J�X��`��ɖ߂�
			return (INT_PTR)TRUE;

	}

	return (INT_PTR)FALSE;
}
//-------------------------------------------------------------------------------------------------

/*!
	�������s
	@param[in]	hDlg	�_�C�����O�n���h��
*/
HRESULT FindExecute( HWND hDlg )
{
//	HWND	hWorkWnd;
	UINT_PTR	dPage, d, h;
	INT		dRange;	//	�����͈́@�O�Ł@�P�t�@�C�� �@�L�����Z�����Q�S�J���t�@�C��
	BOOLEAN	bModCrlf;

	TCHAR	atPattern[MAX_PATH], atBuf[MAX_PATH];


	//	���������s�A���������ɂ��邩
	bModCrlf = IsDlgButtonChecked( hDlg, IDCB_MOD_CRLF_YEN );

	//	�����͈�	�O�Ł@�P�t�@�C��
	dRange = ComboBox_GetCurSel( GetDlgItem(hDlg,IDCB_FIND_TARGET) );
	//
	//�����p���[��
	Edit_GetText( GetDlgItem(hDlg,IDE_FIND_TEXT), atBuf, MAX_PATH );

	if( atBuf[0] )	return  E_ABORT;	//	�󕶎���Ȃ牽�����Ȃ�

	giCrLfCnt = 0;	//	���s�J�E���g�E�ʏ�0����
	ZeroMemory( atPattern, sizeof(atPattern) );
	if( bModCrlf )	//	�G�X�P�[�v�V�[�P���X��W�J
	{
		for( d = 0, h = 0; MAX_PATH > d; d++, h++ )
		{
			atPattern[h] = atBuf[d];	
			if( 0x005C == atBuf[d] )	//	0x005C�́�
			{
				d++;
				if( TEXT('n') ==  atBuf[d] )	//	���s�w���ł���ꍇ
				{
					atPattern[h] = TEXT('\r');	h++;
					atPattern[h] = TEXT('\n');
					giCrLfCnt++;	//	���s�J�E���g���₷
				}
			}
		}
	}
	else
	{
		StringCchCopy( atPattern, MAX_PATH, atBuf );
	}

//�����������O�ƕς�炸�A�S�Ō����Ȃ�A���̃q�b�g�łɈړ�����
//�ς���Ă��邩�A�P�Ō����Ȃ�A���ʂɌ���
	if( !( StrCmp(atPattern,gatNowPtn) ) && gixFindMode == dRange )
	{
		//	���q�b�g�łɈړ�
#error �쐬��
	}
	else
	{
		//	�����ʂ�Ɍ���

		gixFindMode = dRange;

		FindHighlightOff(  );	//	��̃p���[���j��

		//	�Ƃ肠������Search
		if( dRange )	//	�S�Ō�������������肵��
		{
			dPage = gitFileIt->vcCont.size( );
			for( d = 0; dPage > d; d++ )
			{
				FindPageSearch( atPattern, d, gitFileIt );
			}
		}
		else	//	�Ȃ��ł̂�
		{
			FindPageSearch( atPattern, gixFocusPage, gitFileIt );
		}
	}

	return S_OK;
}
//-------------------------------------------------------------------------------------------------

/*!
	�w��p���[�����A�w��t�@�C���́A�w��łŌ���
	@param[in]	ptPattern	�����p���[���ENULL�Ȃ�Ō�̌�����������g�p����
	@param[in]	iTgtPage	�Ώە�
	@param[in]	itFile		�t�@�C���C�e���[�^
	@return		HRESULT	�I����ԃR�[�h�E�q�b�g������S_OK
*/
HRESULT FindPageSearch( LPTSTR ptPattern, INT iTgtPage, FILES_ITR itFile )
{
	HRESULT		hRslt;
	INT			dCch, dLeng;
	INT_PTR		dBytes;
	UINT_PTR	cchSize, cchSzPtn;
	LPTSTR		ptPage, ptCaret, ptFind = NULL;

	TCHAR	ttBuf;

	TRACE( TEXT("PAGE[%d] SEARCH"), iTgtPage );

	if( ptPattern ){	StringCchCopy( gatNowPtn, MAX_PATH, ptPattern );	}	//	���X�g�������[

	StringCchLength( gatNowPtn, MAX_PATH, &cchSzPtn );

	//	�f�B���C���Ă���΃`�F�b�N���Ȃ��Ă���
	if( PageIsDelayed( itFile, iTgtPage ) ){	return  E_ABORT;	}
	//	�q�b�g�����邩�ǂ��������Ԃ��A�ł�������

	//	�őS�̊m��
	dBytes = DocPageTextGetAlloc( itFile, iTgtPage, D_UNI, (LPVOID *)(&ptPage), FALSE );
	StringCchLength( ptPage, STRSAFE_MAX_CCH, &cchSize );

	ptCaret = ptPage;

	//	��������
	hRslt = E_NOTIMPL;
	dLeng = 0;
	do
	{
		ptFind = FindStringProc( ptCaret, gatNowPtn, &dCch );	//	�q�b�g������擪����������
		if( !(ptFind) ){	break;	}	//	�q�b�g���Ȃ�����

		dLeng += dCch;	//	�����ʒu�E�O�C���f�b�N�X
		ttBuf = ptPage[dLeng];

		//	�q�b�g���������ɐF��t����
		FindPageHighlightSet( dLeng, cchSzPtn, iTgtPage, itFile );	//	���ŊY���̈�̍ĕ`��܂Ŏw�肵�Ă�

		hRslt = S_OK;

		ptCaret = ptFind;
		ptCaret++;

		dLeng++;

	}while( *ptCaret );

	FREE(ptPage);

	return hRslt;
}
//-------------------------------------------------------------------------------------------------

/*!
	�w��t�@�C���̎w��ł̎w�蕶���ʒu����w�蕶�������n�C���C�g�w��ɂ���B���s�R�[�h�܂ށB
	@param[in]	iOffset	�Ő擪����̕������E���s�R�[�h�܂ށB
	@param[in]	iRange	�n�C���C�g������
	@param[in]	iPage	�Ώە�
	@param[in]	itFile	�t�@�C���C�e���[�^
	@return		HRESULT	�I����ԃR�[�h
*/
HRESULT FindPageHighlightSet( INT iOffset, INT iRange, INT iPage, FILES_ITR itFile )
{
	UINT_PTR	ln, iLetters;//, iLines;
	INT_PTR		dMozis;
	INT			iTotal, iDot, iLnTop, iSlide, mz, iNext, iWid = 0;
	RECT		inRect;

	LINE_ITR	itLine, itLnEnd;

	itLine  = itFile->vcCont.at( iPage ).ltPage.begin();
	itLnEnd = itFile->vcCont.at( iPage ).ltPage.end();

	iTotal = 0;
	iLnTop = 0;
	for( ln = 0; itLnEnd != itLine; itLine++, ln++ )
	{
		inRect.top    = ln * LINE_HEIGHT;
		inRect.bottom = inRect.top + LINE_HEIGHT;
		inRect.left   = 0;
		inRect.right  = 0;

		dMozis = itLine->vcLine.size( );	//	���̍s�̕������m�F����
		iLetters = dMozis + 2;	//	���s�R�[�h

		iTotal += iLetters;

		if( iOffset < iTotal )	//	�s���[�܂ł̕��������I�t�Z�b�g��������������A���̍s�Ɋ܂܂��
		{
			iSlide = iOffset - iLnTop;	//	���̍s�擪����̕�����
			//	�������s���猟����������AiSlide = dMozis �ɂȂ�
			iNext = 0;	//	���s���L��ꍇ�̎c�蕶����

			//	�����ŉ��s�̊������ݏ󋵂��m�F���āA���̍s���Ԃ�Ƃ��`�F�b�N�H
			if( dMozis < (iSlide + iRange) )	//	�����Ȃ�A���s���͂ݏo���Ă�
			{
				iNext  = iRange;
				iRange = dMozis - iSlide;	//	�c�蕶����
				iNext -= iRange;	//	���s���܂߂��c�蕶����
			}

			iDot = 0;	//	�����܂ł̃h�b�g�������ߍ���
			for( mz = 0; iSlide > mz; mz++ )	//	�Y�������܂Ői�߂ăh�b�g�����߂Ƃ�
			{
				//	�������s���猟���Ȃ炱�ꂪ����
				if( dMozis <=  mz ){	iDot += iWid;	break;	}

				iDot += itLine->vcLine.at( mz ).rdWidth;

				iWid  = itLine->vcLine.at( mz ).rdWidth;	//	���̕����̕�
			}

			for(  ; (iSlide+iRange) > mz; mz++ )
			{
				//	���s�܂ł߂荞�ނȂ炱��������
				if( dMozis <=  mz ){	break;	}

				iWid = itLine->vcLine.at( mz ).rdWidth;

				inRect.left  = iDot;
				inRect.right = iDot + iWid;

				itLine->vcLine.at( mz ).mzStyle |= CT_FINDED;
				ViewRedrawSetRect( &inRect );

				iDot += iWid;
			}

			if( 0 <	 iNext )	//	���s������Ȃ琔�l������
			{
				inRect.right += 20;

				itLine->dStyle |= CT_FINDRTN;
				ViewRedrawSetRect( &inRect );

				iNext -=  2;	//	���s�������ւ炵��

			}

			//	�K�v�ɉ����āA�����ŏI������莟�̍s�̏����ɂ�������
			if( 0 >= iNext ){	break;	}
			else
			{
				iOffset += (2 + iRange);	//	���s�������{���̍s�Ŏg�����������i�߂�
				iRange  = iNext;	//	�c�蕶��������
			}
		}

		iLnTop += iLetters;
	}


	return S_OK;
}
//-------------------------------------------------------------------------------------------------

/*!
	�S���n�C���C�g�n�e�e
	@return	HRESULT	�I����ԃR�[�h
*/
HRESULT FindHighlightOff( VOID )
{
	UINT_PTR	dPage, d;

	dPage = gitFileIt->vcCont.size( );

	for( d = 0; dPage > d; d++ )
	{
		FindPageHighlightOff( d, gitFileIt );
	}

	//	���ݓ��e��������
	FindDialogueOpen( NULL, NULL );

	return S_OK;
}
//-------------------------------------------------------------------------------------------------

/*!
	�w��t�@�C���̎w��ł̌����n�C���C�g��OFF����
	@param[in]	iPage	�Ώە�
	@param[in]	itFile	�t�@�C���C�e���[�^
	@return		���ɂȂ�
*/
INT FindPageHighlightOff( INT iPage, FILES_ITR itFile )
{
	UINT_PTR	ln;//, iLines, iLetters, mz;
//	UINT		dStyle;
//	INT			iDot, iWid;
//	RECT		inRect;

	LINE_ITR	itLine, itLnEnd;


	if( 0 >  iPage )	return 0;	//	����ȏ󋵉��ł͏������Ȃ�

	ZeroMemory( gatNowPtn, sizeof(gatNowPtn) );

	itLine  = itFile->vcCont.at( iPage ).ltPage.begin();
	itLnEnd = itFile->vcCont.at( iPage ).ltPage.end();

	for( ln = 0; itLnEnd != itLine; itLine++, ln++ )
	{
		FindLineHighlightOff( ln, itLine );	//	����REDRAW�������Ă�
	}

//	ViewRedrawSetLine( -1 );	//	��ʕ\���X�V

	return 1;
}
//-------------------------------------------------------------------------------------------------

/*!
	�w��s�̌����n�C���C�g��OFF����
	@param[in]	iLine	�s���E�O�C���f�b�N�X
	@param[in]	itFile	�s�C�e���[�^
	@return	HRESULT	�I����ԃR�[�h
*/
HRESULT FindLineHighlightOff( UINT iLine, LINE_ITR itLine )
{
	UINT_PTR	iLetters, mz;//, iLines, ln;
	UINT		dStyle;
	INT			iDot, iWid;
	RECT		inRect;


	iDot = 0;	//	�����܂ł̃h�b�g�������ߍ���
	inRect.top    = iLine * LINE_HEIGHT;
	inRect.bottom = inRect.top + LINE_HEIGHT;

	iLetters = itLine->vcLine.size();
	//	�땶�����A�S�����`�F�L���Ă���
	for( mz = 0; iLetters > mz; mz++ )
	{
		//	���O�̏��
		dStyle = itLine->vcLine.at( mz ).mzStyle;
		iWid   = itLine->vcLine.at( mz ).rdWidth;

		inRect.left  = iDot;
		inRect.right = iDot + iWid;

		itLine->vcLine.at( mz ).mzStyle &= ~CT_FINDED;
		if( dStyle & CT_FINDED )	ViewRedrawSetRect( &inRect );

		iDot += iWid;
	}

	//	��s�I������疖���󋵊m�F�B���s�E�{�����[�ɉ��s�͂Ȃ��E�I���̂Ƃ��̂�
	dStyle = itLine->dStyle;
	inRect.left  = iDot;
	inRect.right = iDot + 20;	//	���s�`��G���A�E��̂��ꂭ�炢

	itLine->dStyle &=  ~CT_FINDRTN;
	if( dStyle & CT_FINDRTN )	ViewRedrawSetRect( &inRect );


	return S_OK;
}
//-------------------------------------------------------------------------------------------------

#if 0
/*!
	�����ʒu�փW�����v
	@param[in]	dMode	�O���ց@�P�O��
	@param[in]	pxDot	�J�[�\���h�b�g�ʒu�E����������߂�
	@param[in]	pyLine	�J�[�\���s�E����������߂�
	@param[in]	pMozi	�L�����b�g�̍����̕������E����������߂�
	@return		INT		���F�G���[�@�O�F�q�b�g�����@�P�`�F�q�b�g����
*/
INT FindStringJump( UINT dMode, PINT pXdot, PINT pYline, PINT pMozi )
{
	INT			iXdot, iYline, iMozi;
	BOOLEAN		bStart, bBegin;
	INT_PTR		dTotalPage, dTotalLine;
	PAGE_ITR	itPage;	//	�ł����Ɍ��Ă���
	LINE_ITR	itLine;	//	�s�����Ɍ��Ă���
	LETR_ITR	itMozi;	//	���������Ɍ��Ă���

	if( !(pXdot) || !(pYline) || !(pMozi) ){	return -1;	}

	//	�ł͂ݏo���`�F�L
	dTotalPage = gitFileIt->vcCont.size();
	if( !(0 <= gixFocusPage && gixFocusPage < dTotalPage) ){	return -1;	}
	//	���̕łɒ���
	itPage = gitFileIt->vcCont.begin();
	std::advance( itPage, gixFocusPage );

	iXdot  = *pXdot;
	iYline = *pYline;
	iMozi  = *pMozi;


	//	�s�͂ݏo���`�F�L
	dTotalLine = itPage->ltPage.size();
	if( !(0 <= iYline && iYline < dTotalLine) ){	return -1;	}
	//	���̍s�ɒ���
	itLine = itPage->ltPage.begin();
	std::advance( itLine, iYline );

	itMozi = itLine->vcLine.begin();
	std::advance( itMozi, iMozi );

	bStart = TRUE;	bBegin = TRUE;
	for( ; itPage->ltPage.end() != itLine; itLine++ )
	{
		//	�ŏ��͂��̂܂܁A���񂩂�͂��̍s�̐擪�������猩�Ă���
		if( bStart ){	bStart =  FALSE;	}
		else{	itMozi = itLine->vcLine.begin();	}

		for( ;itLine->vcLine.end() != itMozi; itMozi++ )
		{
			if( itMozi->mzStyle & CT_FINDED )	//	�q�b�g
			{
				if( !(bBegin)  )	//	�����J�n�ʒu�͖̂�������
				{
					*pXdot  = iXdot;
					*pYline = iYline;
					*pMozi  = iMozi;

					ViewDrawCaret( iXdot, iYline, TRUE );	//	�L�����b�g���ړ�

					return 1;
					//	�ł܂����͂ǂ������
				}
			}
			else{	bBegin =  FALSE;	}
			//	

			iXdot += itMozi->rdWidth;
			iMozi++;
		}

		iYline++;	//	�s�ԍ��͑��₷
		iXdot = 0;
		iMozi = 0;
	}

	//	�ł܂����͂ǂ������

	return 0;
}
//-------------------------------------------------------------------------------------------------
#endif

/*!
	���ł��Č����E��ʍĕ`��Ƃ�
	@return	HRESULT	�I����ԃR�[�h
*/
HRESULT FindNowPageReSearch( VOID )
{
	FindHighlightOff(  );

	FindPageSearch( gatNowPtn, gixFocusPage, gitFileIt );	//	�d��

	return S_OK;
}
//-------------------------------------------------------------------------------------------------

/*!
	�x���Ǎ��ł��J���Ƃ��ɁA�����n�C���C�c�̖ʓ|����
	@param[in]	iTgtPage	�ΏەŔԍ�
	@return	HRESULT	�I����ԃR�[�h
*/
HRESULT FindDelayPageReSearch( INT iTgtPage )
{

	//	�S�̌����łȂ����A���������񂪋�Ȃ疳�����Ă�낵
	if(  1 != gixFindMode || NULL == gatNowPtn[0] ){	return  E_ABORT;	}


	//	�Ƃ肠������Search
	FindPageSearch( NULL, iTgtPage, gitFileIt );


	return S_OK;
}
//-------------------------------------------------------------------------------------------------

#if 0
/*!
	�������}���폜���ꂽ�Ƃ��ɁA���̕�����Check������
	@param[in]	iTgtLine	�Ώۍs�ԍ�
	@return	HRESULT	�I����ԃR�[�h
*/
HRESULT FindTextModifyLine( INT iTgtLine )
{
//�Y���s�Ɖ��s���y�Ԕ͈͂̍s�̂݁A��������`�F�b�N�O���āA�Y���͈͂̍s�����`�F�L
//�R�s�y���ꂽ�ꍇ�́H�@�}���폜�������s���Ă���`�F�b�N����Ȃ�A�e�����咍��

	//	giCrLfCnt

	//�s���m�F�E�͂ݏo���Ȃ��悤��


	return S_OK;
}
//-------------------------------------------------------------------------------------------------
#endif

#endif	//	FIND_STRINGS

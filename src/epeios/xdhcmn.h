/*
	Copyright (C) 2000-2015 Claude SIMON (http://q37.info/contact/).

	This file is part of the Epeios framework.

	The Epeios framework is free software: you can redistribute it and/or
	modify it under the terms of the GNU Affero General Public License as
	published by the Free Software Foundation, either version 3 of the
	License, or (at your option) any later version.

	The Epeios framework is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
	Affero General Public License for more details.

	You should have received a copy of the GNU Affero General Public License
	along with the Epeios framework.  If not, see <http://www.gnu.org/licenses/>
*/

#ifndef XDHCMN__INC
# define XDHCMN__INC

# define XDHCMN_NAME		"XDHCMN"

# if defined( E_DEBUG ) && !defined( XDHCMN_NODBG )
#  define XDHCMN_DBG
# endif

// X(SL)/DH(TML) CoMmoN

# include "err.h"
# include "flw.h"
# include "ntvstr.h"
# include "strmrg.h"
# include "sclmisc.h"

# include <stdarg.h>

# define XDHCMN_SHARED_DATA_VERSION_NUMBER	"11"

# define XDHCMN_SHARED_DATA_VERSION			XDHCMN_SHARED_DATA_VERSION_NUMBER "-" CPE_AL

# define XDHCMN_RETRIEVE_FUNCTION_NAME		XDHCMNRetrieve

namespace xdhcmn {
	// NOTA : not used by this module, but by other 'upstream' and 'downtream' 'XDH...' modules.
	typedef ntvstr::char__ nchar__;
	typedef ntvstr::string___ nstring___;
	typedef ntvstr::rString rNString;
	qMIMICs( bso::sU16, sIndex );	// For the 'CSS' rules.

	static E_CDEF( char *, CloseActionLabel, "Q37Close" );
	static E_CDEF( char *, RefreshActionLabel, "Q37Refresh" );

	qENUM( Mode ) {
		mMonoUser,	// One use only. The content of the project (i.e. which backend type to use) is defined by the user.
		mMultiUser,	// Several users. The project (which defines the backend to use) is predefiend in the configuration file.
		m_amount,
		m_Undefined
	};

	class cSession
	{
	protected:
		virtual bso::bool__ XDHCMNLaunch(
			const char *Id,
			const char *Action ) = 0;
	public:
		qCALLBACK( Session );
		bso::bool__ Launch(
			const char *Id,
			const char *Action )
		{
			return XDHCMNLaunch( Id, Action );
		}
	};

	enum function__ {		// Parameters :
		fExecute,			// Script,
		fLog,				// Message,
		fAlert,				// XML, XSL, Title.
		fConfirm,			// XML, XSL, Title.
		fSetLayout,			// Id, XML, XSL.
		fSetContents,		// Ids, Contents.
		fDressWidgets,		// Id.
		fInsertCSSRule,		// Rule, Index.
		fAppendCSSRule,		// Rule.
		fRemoveCSSRule,		// Index.
		fAddClasses,		// Ids, Classes.
		fRemoveClasses,		// Ids, Classes.
		fToggleClasses,		// Ids, Classes.
		fEnableElements,	// Ids.
		fDisableElements,	// Ids.
		fSetProperty,		// Id, Name, Value.
		fGetProperty,		// Id, Name.
		fSetAttribute,		// Id, Name, Value.
		fGetAttribute,		// Id, Name.
		fRemoveAttribute,	// Id, Name.
		fSetValue,			// Id, Value.
		fGetValue,			// Id.
		fGetResult,			// Id.
		fFocus,				// Id.
		fSelect,			// Id.
		f_amount,
		f_Undefined
	};

	const char *GetLabel( function__ Function );

	class cProxy
	{
	protected:
		virtual void XDHCMNProcess(
			function__ Function,
			TOL_CBUFFER___ *Result,
			va_list List ) = 0;
	public:
		qCALLBACK( Proxy );
		void Process(
			function__ Function,
			TOL_CBUFFER___ *Result,
			...	)
		{
		qRH
			va_list List;
		qRB
			va_start( List, Result );

			XDHCMNProcess( Function, Result, List );
		qRR
		qRT
			va_end( List );
		qRE
		}
	};

#pragma pack( push, 1)
	// NOTA : is modified, increment 'CSDLEO_SHARED_DATA_VERSION' !
	class shared_data__
	{
	private:
		const char *_Version;	// Toujours en premire position.
		bso::uint__ _Control;	// Une valeur relative au contenu de la structure,  des fins de test primaire de compatibilit.
		eMode _Mode;
		const char *_LauncherIdentification;
		const char *_Localization;
		sclmisc::sRack SCLRack_;
	public:
		void reset( bso::bool__ P = true )
		{
			_Version = NULL;
			_Control = 0;
			_Mode = m_Undefined;
			_LauncherIdentification = NULL;
			_Localization = NULL;
			SCLRack_.reset( P );
		}
		E_CDTOR( shared_data__ );
		void Init(
			eMode Mode,
			const char *LauncherIdentification,
			const char *Localization )
		{
			_Version = XDHCMN_SHARED_DATA_VERSION;
			_Control = ControlComputing();
			_Mode = Mode;
			_LauncherIdentification = LauncherIdentification;
			_Localization = Localization;
			SCLRack_.Init();
		}
		size_t ControlComputing( void )
		{
			return sizeof( shared_data__ );
		}
		qRWDISCLOSEs( sclmisc::sRack, SCLRack );
		Q37_PMDF( const char, LauncherIdentification, _LauncherIdentification );
		Q37_PMDF( const char, Localization, _Localization );
		E_RODISCLOSE__( eMode, Mode );
	};
#pragma pack( pop )

	class cDownstream
	{
	protected:
		virtual void XDHCMNInitialize( const shared_data__ &Data ) = 0;
		virtual void XDHCMNBaseLanguage( TOL_CBUFFER___ &Buffer ) = 0;
		virtual cSession *XDHCMNRetrieveCallback(
			const char *Language,
			cProxy *Proxy ) = 0;
		virtual void XDHCMNReleaseCallback( cSession *Session ) = 0;
	public:
		qCALLBACK( Downstream )
		void Initialize( const shared_data__ &Data )
		{
			return XDHCMNInitialize( Data );
		}
		const char *BaseLanguage( TOL_CBUFFER___ &Buffer )
		{
			XDHCMNBaseLanguage( Buffer );

			return Buffer;
		}
		cSession *RetrieveCallback(
			const char *Language,
			cProxy *Proxy )
		{
			return XDHCMNRetrieveCallback( Language, Proxy );
		}
		void ReleaseCallback( cSession *Session )
		{
			return XDHCMNReleaseCallback( Session );
		}
	};

	typedef cDownstream *(retrieve)( void );

	void Escape(
		const str::string_ &Source,
		str::string_ &Target,
		bso::char__ Delimiter,	// Should be '\'', '"' or 0. If 0, escapes '\'' and '\"', otherwise escapes 'Delimiter'.
		bso::char__ EscapeChar = strmrg::DefaultEscapeToken );
#if 0
	void Unescape(
		const str::string_ &Source,
		str::string_ &Target,
		bso::char__ EscapeChar = DefaultEscapeChar );
#endif

	typedef strmrg::table_ digest_;
	E_AUTO( digest );

	using strmrg::Split;
	using strmrg::Merge;

	using strmrg::retriever__;

	// Merge 'Splitted', delimited by '"', and separated by ','.
	void FlatMerge(
		const str::dStrings &Splitted,
		str::dString &Merged,
		bso::sBool AsJSArray );	// If 'true', the splitted items are enclosed between '[]', so the result can be used as JS array; above function does then NOT work.

	// Reverse of above (if 'AsJSArray' was to 'false').
	void FlatSplit(
		flw::sRFlow &Flow,
		str::dStrings &Splitted );

	// Variant of above.
	void FlatSplit(
		const str::dString &Merged,
		str::dStrings &Splitted );
}

#endif

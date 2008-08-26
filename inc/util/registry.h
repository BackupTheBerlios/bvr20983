/*
 * $Id$
 * 
 * Copyright (C) 2008 Dorothea Wachmann
 * 
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see http://www.gnu.org/licenses/.
 */
#if !defined(REGISTRY_H)
#define REGISTRY_H

#include "os.h"
#include "util/logstream.h"
#include <iostream>
#include <fstream>
#include <stack>

namespace bvr20983
{

  /**
   *
   */
  class RegistryValue
  {
    public:
      RegistryValue();
      RegistryValue(const RegistryValue& val);
      RegistryValue(LPCTSTR name,LPCTSTR value,DWORD type=REG_SZ);
      RegistryValue(LPCTSTR name,DWORD value,DWORD type=REG_SZ);
      ~RegistryValue();

      void GetValue(TString &value);
      void GetValue(DWORD& value);

      LPCTSTR GetName() const
      { return m_name.c_str(); }

      DWORD GetType() const
      { return m_type; }

      DWORD GetSize() const
      { return NULL!=m_pValue ? (m_pValue->length()+1)*sizeof(TCHAR) : sizeof(m_intValue); }

      LPBYTE GetBuffer() const
      { return NULL!=m_pValue ? (LPBYTE)(const_cast<LPTSTR>(m_pValue->c_str())) : (LPBYTE)&m_intValue; }

      RegistryValue& operator=(const RegistryValue& val);

    private:
      TString  m_name;
      TString* m_pValue;
      DWORD    m_intValue;
      DWORD    m_type;
  }; // of class RegistryValue

  /**
   *
   */
  class RegKey
  {
    public:
      RegKey(const TString& path);
      RegKey(LPCTSTR path);
      ~RegKey();
      
      bool     Create();
      bool     Open() const;
      void     Close();
      void     Delete(bool deep=false);
      bool     Exists() const;
      bool     HasSubKey() const;
      bool     EnumKey(TString& keyName,DWORD index) const;
      bool     QueryValue(LPCTSTR name,RegistryValue &value) const;
      void     SetValue(const RegistryValue& value);
      operator TString() const;

    private:
      void Init(LPCTSTR path);
    
      HKEY     m_mainKey;
      HKEY     m_key;
      TString  m_subKey;
  }; // of class RegKey

  /**
   *
   */
  class RegKeyEnum
  {
    public:
      RegKeyEnum(LPCTSTR path,UINT32 maxDepth=1);
      RegKeyEnum(const TString& path,UINT32 maxDepth=1);

      bool Next(TString& keyName);
      
    private:
      struct State
      { RegKey m_key;
        DWORD  m_index;
        
        State(RegKey key) : m_key(key),m_index(0) {}
      };

      stack<State> m_stack;
      DWORD        m_maxDepth;
  }; // of class RegKeyEnum

  /**
   *
   */
  class RegistryKey
  {
    public:
      RegistryKey(LPCTSTR path=NULL);
      RegistryKey(const TString& path);
      RegistryKey(const RegistryKey& key);
      ~RegistryKey();
      
      void     SetValue(LPCTSTR name,LPCTSTR value);
      void     SetValue(LPCTSTR name,DWORD value);
      bool     QueryValue(LPCTSTR name,RegistryValue& value) const;
      bool     Prepare() const;
      bool     Commit();

      RegistryKey& operator=(const RegistryKey& val);

      const TString GetKey() const
      { return m_key; }

    private:
      typedef std::pair<LPCTSTR,RegistryValue>         RegistryValueP;
      typedef std::map <LPCTSTR,RegistryValue,strless> RegistryValueM;

      TString         m_key;
      RegistryValueM  m_values;
  }; // of class RegistryKey

  template<class charT, class Traits>
  std::basic_ostream<charT, Traits>& operator <<(std::basic_ostream<charT, Traits >& os,const RegistryKey& rKey);

  /**
   *
   */
  class Registry
  {
    public:
      Registry(const TString& keyPrefix)
      { m_keyPrefix = keyPrefix; }

      Registry(LPCTSTR keyPrefix=NULL)
      { if( NULL!=keyPrefix) m_keyPrefix = keyPrefix; }

      void SetValue  (LPCTSTR subkey,LPCTSTR name,const TString& value,DWORD type=REG_SZ);
      void SetValue  (LPCTSTR subkey,LPCTSTR name,DWORD value,DWORD type=REG_DWORD);
      bool QueryValue(LPCTSTR subkey,LPCTSTR name,RegistryValue& value) const;

      void SetKeyPrefix(const TString& keyPrefix)
      { m_keyPrefix = keyPrefix; }

      void SetKeyPrefix(LPCTSTR keyPrefix)
      { m_keyPrefix = keyPrefix; }

      void GetKeyPrefix(TString& keyPrefix) const
      { keyPrefix = m_keyPrefix; }

      void SetDumpFile(LPCTSTR dumpFileName)
      { m_dumpFileName = dumpFileName; }

      bool Prepare() const;
      bool Commit();
      bool Rollback();
      
    private:
      typedef std::pair<LPCTSTR,RegistryKey>         RegistryKeyP;
      typedef std::map <LPCTSTR,RegistryKey,strless> RegistryKeyM;

      void GetKeyPath(LPCTSTR subkey,TString& keyPath) const;

      RegistryKeyM m_keys;
      TString      m_keyPrefix;
      TString      m_dumpFileName;
  }; // of class Registry
} // of namespace bvr20983

#endif // REGISTRY_H
//=================================END-OF-FILE==============================
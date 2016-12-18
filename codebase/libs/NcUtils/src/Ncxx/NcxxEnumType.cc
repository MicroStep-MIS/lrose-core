#include <NcUtils/NcxxEnumType.hh>
#include <NcUtils/NcxxGroup.hh>
#include <NcUtils/NcxxCheck.hh>
#include <NcUtils/NcxxByte.hh>
#include <NcUtils/NcxxUbyte.hh>
#include <NcUtils/NcxxChar.hh>
#include <NcUtils/NcxxShort.hh>
#include <NcUtils/NcxxUshort.hh>
#include <NcUtils/NcxxInt.hh>
#include <NcUtils/NcxxUint.hh>
#include <NcUtils/NcxxInt64.hh>
#include <NcUtils/NcxxUint64.hh>
#include <NcUtils/NcxxFloat.hh>
#include <NcUtils/NcxxDouble.hh>
#include <NcUtils/NcxxString.hh>
#include <NcUtils/NcxxException.hh>
using namespace std;
using namespace netCDF;
using namespace netCDF::exceptions;

// Class represents a netCDF variable.

// assignment operator
NcxxEnumType& NcxxEnumType::operator=(const NcxxEnumType& rhs)
{
  NcxxType::operator=(rhs);    // assign base class parts
  return *this;
}

// assignment operator
NcxxEnumType& NcxxEnumType::operator=(const NcxxType& rhs)
{
  if (&rhs != this) {
    // check the rhs is the base of an Enum type
    if(getTypeClass() != NC_ENUM) throw NcxxException("The NcxxType object must be the base of an Enum type.",__FILE__,__LINE__);
    // assign base class parts
    NcxxType::operator=(rhs);
  }
  return *this;
}

// The copy constructor.
NcxxEnumType::NcxxEnumType(const NcxxEnumType& rhs): 
  NcxxType(rhs)
{
}


// Constructor generates a null object.
NcxxEnumType::NcxxEnumType() :
  NcxxType()   // invoke base class constructor
{}
  
// constructor
NcxxEnumType::NcxxEnumType(const NcxxGroup& grp, const string& name):
  NcxxType(grp,name)
{}
  

// constructor
NcxxEnumType::NcxxEnumType(const NcxxType& ncType): 
  NcxxType(ncType)
{
  // check the nctype object is the base of an Enum type
  if(getTypeClass() != NC_ENUM) throw NcxxException("The NcxxType object must be the base of an Enum type.",__FILE__,__LINE__);
}

// Returns the base type.
NcxxType NcxxEnumType::getBaseType() const
{
  char charName[NC_MAX_NAME+1];
  nc_type base_nc_typep;
  size_t *base_sizep=NULL;
  size_t *num_membersp=NULL;
  ncxxCheck(nc_inq_enum(groupId,myId,charName,&base_nc_typep,base_sizep,num_membersp),__FILE__,__LINE__);
  switch (base_nc_typep) {
  case NC_BYTE    : return ncByte;
  case NC_UBYTE   : return ncUbyte;
  case NC_CHAR    : return ncChar;
  case NC_SHORT   : return ncShort;
  case NC_USHORT  : return ncUshort;
  case NC_INT     : return ncInt;
  case NC_UINT    : return ncUint;  
  case NC_INT64   : return ncInt64; 
  case NC_UINT64  : return ncUint64;
  case NC_FLOAT   : return ncFloat;
  case NC_DOUBLE  : return ncDouble;
  case NC_STRING  : return ncString;
  default:  
    // this is a user defined type
    return NcxxType(getParentGroup(),base_nc_typep);
  }
}

  
// Returns number of members in this NcxxEnumType object.
size_t   NcxxEnumType::getMemberCount() const{
  char charName[NC_MAX_NAME+1];
  nc_type* base_nc_typep=NULL;
  size_t* base_sizep=NULL;
  size_t num_membersp;
  ncxxCheck(nc_inq_enum(groupId,myId,charName,base_nc_typep,base_sizep,&num_membersp),__FILE__,__LINE__);
  return num_membersp;
};
  
// Returns the member name for the given zero-based index.
string NcxxEnumType::getMemberNameFromIndex(int index) const{
  void* value=NULL;
  char charName[NC_MAX_NAME+1];
  ncxxCheck(nc_inq_enum_member(groupId,myId,index,charName,value),__FILE__,__LINE__);
  return static_cast<string> (charName);
};

#pragma once
#define  BOOST_ALL_NO_LIB
namespace boost
{
	namespace archive
	{
		class text_iarchive;
		class text_oarchive;
		class polymorphic_iarchive;
		class polymorphic_oarchive;
		
	};
	namespace serialization
	{
		class access;
	};
};


#define DECLARE_SERIALIZE friend class boost::serialization::access;\
void serialize(boost::archive::text_iarchive &ar, const unsigned int file_version);\
void serialize(boost::archive::text_oarchive &ar, const unsigned int file_version);

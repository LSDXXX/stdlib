#pragma once
#define MK_TRIVAL_TYPE(Ty) \
template < >\
struct is_trival_type<Ty>\
{										\
enum { result = 1 };			\
}										
namespace lsd
{
	template <typename Ty>
	struct  is_trival_type
	{
		enum { result = 0 };
	};
	template <typename Ty>
	struct is_trival_type<Ty*>
	{
		enum { result = 1 };
	};
	MK_TRIVAL_TYPE(int);
	MK_TRIVAL_TYPE(char);
	MK_TRIVAL_TYPE(unsigned int);
	MK_TRIVAL_TYPE(long);
	MK_TRIVAL_TYPE(unsigned long);
	MK_TRIVAL_TYPE(unsigned char);
	MK_TRIVAL_TYPE(short);
	MK_TRIVAL_TYPE(unsigned short);
	MK_TRIVAL_TYPE(double);
	MK_TRIVAL_TYPE(float);
}
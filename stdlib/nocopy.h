#pragma once
namespace lsd
{
	class NoCopy
	{
	public:
		NoCopy(){}
	private:
		NoCopy(const NoCopy&);
		NoCopy& operator=(const NoCopy&);
	};
}
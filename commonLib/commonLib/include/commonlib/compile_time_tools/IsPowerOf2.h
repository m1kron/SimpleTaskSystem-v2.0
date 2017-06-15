#pragma once

/////////////////////////////////////////////////////////
// IsPowerOf2::value will hold 1 if number_to_check is indeed power of 2, 0 otherwise
template< unsigned long long number_to_check >
struct IsPowerOf2
{
	enum { value = ( number_to_check & ( number_to_check - 1 ) ) == 0 };
};

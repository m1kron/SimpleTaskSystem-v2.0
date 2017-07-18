#pragma once

NAMESPACE_STS_BEGIN
NAMESPACE_COMMON_BEGIN

// Performs hashing of positive integral numbers. Prime number have to be *much* bigger than max_number.
// Returns hashed number clamped to [ 0, max_number ].
template < uint32_t PrimeNumber = 2654435761 >
uint32_t CalcHashedNumberClamped( uint32_t number_to_hash, uint32_t max_number )
{
	return ( ( number_to_hash * PrimeNumber ) ) % max_number;
}

NAMESPACE_COMMON_END
NAMESPACE_BTL_END
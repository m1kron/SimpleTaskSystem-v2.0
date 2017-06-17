#pragma once

// Performs hashing of positive integral numbers. Prime number have to be *much* bigger than max_number.
// Returns hashed number from number_to_hash.
template < uint32_t MaxNumber, uint32_t PrimeNumber = 2654435761 >
uint32_t CalcHashedNumber( uint32_t number_to_hash )
{
	return ( ( number_to_hash * PrimeNumber ) ) % MaxNumber;
}
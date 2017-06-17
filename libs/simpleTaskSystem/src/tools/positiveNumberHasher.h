#pragma once

// Performs hashing of positive integral numbers. Prime number have to be *much* bigger than max_number.
// Returns hashed number from number_to_hash.
template < unsigned MaxNumber, unsigned PrimeNumber = 2654435761 >
unsigned CalcHashedNumber( unsigned number_to_hash )
{
	return ( ( number_to_hash * PrimeNumber ) ) % MaxNumber;
}
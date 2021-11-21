import pytest
import DNA
import array
import numpy as np

###### Verify if C extension in Python is working #####

def test_DNA_generating_mRNA():
	res = DNA.generating_mRNA(array.array('i', [0,1,0,0,1,1,0,0,0,1,1,0]))
	assert res == "GAUAGC"

def test_DNA_detecting_genes():
	a = DNA.detecting_genes(array.array('I', [0,0,1,1,1,0,1,1,0,0,0,0]),12)
	assert a == [[0,6]]

#Maybe not good value, I don't know
def test_DNA_generating_amino_acid_chain():
	res = DNA.generating_amino_acid_chain("AUGAGC", 6, 1)
	assert res == None

def test_DNA_detecting_mutations():
	res = DNA.detecting_mutations(array.array('I', [0,1,0,0,1,1,0,0,0,1,1,0]))
	assert res

def test_DNA_calculating_matching_score():
	res = DNA.calculating_matching_score(array.array('i', [0,1,0,0,1,1,0,0,0,1,1,0]), array.array('i', [1,0,1,1,0,0,1,1,1,0,0,1]))
	assert res == -1.0

def test_DNA_hamming():
	res = DNA.hamming(0b010011000110, 0b101100111001)
	assert res == 12


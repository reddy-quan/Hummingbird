/*
 * (C) Copyright 2012
 *     wangflord@allwinnertech.com
 *
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program;
 *
 */

/*
A. ���ܽ���
1. ��Կ�Ĳ���
	1) �ҳ���������Ĵ�����P��Q����N��P��Q��M����P��1��*��Q��1����
	2) �ҳ���M���صĴ���E����E<M����ŷ���㷨���������D��ʹD��E��1 MOD M��
	3) ����P��Q������E��D��N��E��N��������Կ(��Կ)��D��N��������Կ(˽Կ)��
2. ���ܵĲ���
	1) ����N����Чλ��tn�����ֽ����ƣ��������λ������Ե�����tn1��tn��1������N��0x012A05������Чλ��tn��5��tn1��4
	2) ����������A�ָ��tn1λ�����ֽ����ƣ��Ŀ飬ÿ�鿴��һ��������������Ϊbn���Ӷ�����֤��ÿ�鶼С��N��
	3) ��A��ÿһ��Ai����Bi��Ai^E MOD N���㡣Bi�����������ݵ�һ�飬���������Ŀ�ϲ��������͵õ�����������B��
3. ���ܵĲ���
	1) ͬ���ܵĵ�һ����
	2) ����������B�ָ��tnλ�����ֽ����ƣ��Ŀ飬ÿ�鿴��һ��������������Ϊbn��
	3) ��B��ÿһ��Bi����Ci��Bi^D MOD N���㡣Ci�����������ݵ�һ�飬���������Ŀ�ϲ��������͵õ�����������C��
*/
#include "include.h"


typedef struct public_key_pairs_t
{
	unsigned int  public_key;     // e
	unsigned int  divider;		// n
}
public_key_pairs;

typedef struct private_key_pairs_t
{
	unsigned int  private_key;    // d
	unsigned int  divider;		// n
}
private_key_pairs;

#define  P   (127)
#define  Q   (401)
#define  N   ((P) * (Q))
#define  M   ((P-1) * (Q-1))
#define  E   (53)

public_key_pairs   pblc_keys;
private_key_pairs  prvt_keys;

void rsa_dump(void);

static unsigned int  probe_gcd(unsigned int  divdend, unsigned int  divder)
{
	unsigned int  ret = divdend % divder;

	while(ret)
	{
		divdend = divder;
		divder  = ret;
		ret = divdend % divder;
	}

	return divder;
}

unsigned probe_high_level_power_mod(unsigned int  base_value, unsigned int  power, unsigned int  divider)
{
	unsigned int ret = 1;

	base_value %= divider;
	while(power > 0)
	{
		if(power & 1)
		{
			ret = (ret * base_value) % divider;
		}
		power /= 2;
		base_value = (base_value * base_value) % divider;
	}

	return ret;
}

unsigned rsa_init(void)
{
	unsigned int k;
	unsigned int product;
	unsigned int m_value;

	m_value = M;

	k = 1;
	if(probe_gcd(m_value, E) == 1)		//e,M����
	{
		do
		{
			product = M * k + 1;
			if(!(product % E))
			{
				pblc_keys.public_key = E;
				pblc_keys.divider = N;

				prvt_keys.private_key = product/E;
				prvt_keys.divider = N;

#ifdef DEBUG_MODE
				rsa_dump();
#endif

				return 0;
			}
			k ++;
		}
		while(1);
	}

	return -1;
}


void rsa_dump(void)
{
	__inf("base value\n");
	__inf("M = %d(%d * %d), N = %d(%d * %d)\n", M, P-1, Q-1, N, P, Q);

	__inf("public key: \n");
	__inf("{e, n} = %d, %d\n", pblc_keys.public_key, pblc_keys.divider);

	__inf("private key: \n");
	__inf("{d, n} = %d, %d\n", prvt_keys.private_key, prvt_keys.divider);
}

void rsa_encrypt( unsigned int *input, unsigned int length, unsigned int *output )
{
	unsigned int i;

	for(i=0;i<length;i++)
	{
		output[i] = probe_high_level_power_mod(input[i], pblc_keys.public_key, pblc_keys.divider);
	}

	return ;
}


void rsa_decrypt( unsigned int *input, unsigned int length, unsigned int *output )
{
	unsigned int i;

	for(i=0;i<length;i++)
	{
		output[i] = probe_high_level_power_mod(input[i], prvt_keys.private_key, prvt_keys.divider);
	}

	return ;
}




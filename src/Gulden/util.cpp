// Copyright (c) 2015-2017 The Gulden developers
// Authored by: Malcolm MacLeod (mmacleod@webmail.co.za)
// Distributed under the GULDEN software license, see the accompanying
// file COPYING

#include "util.h"
#include "wallet/wallet.h"
#include "validation.h"
#include "consensus/validation.h"

#include "txdb.h"

#include "primitives/transaction.h"


static bool alreadyInRescan = false;
void rescanThread()
{
    if (alreadyInRescan)
        return;
    alreadyInRescan = true;
    pactiveWallet->ScanForWalletTransactions(chainActive.Genesis(), true);
    alreadyInRescan = false;
}

std::string StringFromSeedType(CHDSeed* seed)
{
    switch(seed->m_type)
    {
        case CHDSeed::CHDSeed::BIP32:
            return "BIP32";
        case CHDSeed::CHDSeed::BIP32Legacy:
            return "BIP32 Legacy";
        case CHDSeed::CHDSeed::BIP44:
            return "BIP44";
        case CHDSeed::CHDSeed::BIP44External:
            return "BIP44 External";
        case CHDSeed::CHDSeed::BIP44NoHardening:
            return "BIP44 No Hardening";
    }
    return "unknown";
}

CHDSeed::SeedType SeedTypeFromString(std::string type)
{
    if (type == "BIP32")
        return CHDSeed::CHDSeed::BIP32;
    else if (type == "BIP32 Legacy" || type == "Bip32L")
        return CHDSeed::CHDSeed::BIP32Legacy;
    else if (type == "BIP44")
        return CHDSeed::CHDSeed::BIP44;
    else if (type == "BIP44 External" || type == "BIP44E")
        return CHDSeed::CHDSeed::BIP44External;
    else if (type == "BIP44 No Hardening" || type == "BIP44NH")
        return CHDSeed::CHDSeed::BIP44NoHardening;

    throw std::runtime_error("Invalid seed type");

    return CHDSeed::CHDSeed::BIP44;
}



// Phase 2 becomes active after 75% of miners signal upgrade.
// After activation creation of 'backwards compatible' PoW2 addresses becomes possible.
std::map<uint256, bool> phase2ActivationCache;
bool IsPow2Phase2Active(const CBlockIndex* pIndex, const Consensus::Params& params)
{
    LOCK2(cs_main, pactiveWallet?&pactiveWallet->cs_wallet:NULL);

    if (!pIndex)
        return false;

    if (phase2ActivationCache.find(pIndex->GetBlockHashLegacy()) != phase2ActivationCache.end())
        return phase2ActivationCache[pIndex->GetBlockHashLegacy()];

    phase2ActivationCache[pIndex->GetBlockHashLegacy()] = (VersionBitsState(pIndex, params, Consensus::DEPLOYMENT_POW2_PHASE2, versionbitscache) == THRESHOLD_ACTIVE);
    return phase2ActivationCache[pIndex->GetBlockHashLegacy()];
}


// Phase 3 becomes active after 200 or more witnessing addresses are present on the chain, as well as a combined witness weight of 20 000 000 or more.
// 'backwards compatible' witnessing becomes possible at this point.

// prevhash of blocks continue to point to previous PoW block alone.
// prevhash of witness block is stored in coinbase.
std::map<uint256, bool> phase3ActivationCache;
uint256 phase3ActivationHash;
bool IsPow2Phase3Active(const CBlockIndex* pIndex,  const CChainParams& chainparams)
{
    LOCK2(cs_main, pactiveWallet?&pactiveWallet->cs_wallet:NULL);

    if (!pIndex)
        return false;

    if (phase3ActivationCache.find(pIndex->GetBlockHashLegacy()) != phase3ActivationCache.end())
        return phase3ActivationCache[pIndex->GetBlockHashLegacy()];

    if (phase3ActivationHash == uint256())
        phase3ActivationHash = ppow2witdbview->GetPhase3ActivationHash();
    if (phase3ActivationHash != uint256())
    {
        const CBlockIndex* pIndexPrev = pIndex;
        while (pIndexPrev)
        {
            if (pIndexPrev->GetBlockHashPoW2() == phase3ActivationHash)
            {
                phase3ActivationCache[pIndex->GetBlockHashLegacy()] = true;
                return true;
            }
            pIndexPrev = pIndexPrev->pprev;
        }
    }

    int64_t nNumWitnessAddresses;
    int64_t nTotalWeight;

    GetPow2NetworkWeight(pIndex, chainparams, nNumWitnessAddresses, nTotalWeight);

    const int64_t nNumWitnessAddressesRequired = IsArgSet("-testnet") ? 10 : 200;
    const int64_t nTotalWeightRequired = IsArgSet("-testnet") ? 2000000 : 20000000;
    if (nNumWitnessAddresses >= nNumWitnessAddressesRequired && nTotalWeight > nTotalWeightRequired)
    {
        phase3ActivationCache[pIndex->GetBlockHashLegacy()] = true;
        ppow2witdbview->SetPhase3ActivationHash(phase3ActivationHash);
        return true;
    }
    phase3ActivationCache[pIndex->GetBlockHashLegacy()] = false;
    return false;
}

// Phase 4 becomes active after witnesses signal that 95% of peers are upgraded, for an entire 'signal window'
// Creation of new 'backwards compatible' witness addresses becomes impossible at this point.
// Full 'backwards incompatible' witnessing triggers at this point.
// New 'segsig' transaction format triggers at this point.
// All peers of version < 2.0 can no longer transact at this point.

// prevhash of blocks starts to point to witness header instead of PoW header
std::map<uint256, bool> phase4ActivationCache;
uint256 phase4ActivationHash;
bool IsPow2Phase4Active(const CBlockIndex* pIndex, const Consensus::Params& params)
{
    LOCK2(cs_main, pactiveWallet?&pactiveWallet->cs_wallet:NULL);

    if (!pIndex)
        return false;

    if (phase4ActivationCache.find(pIndex->GetBlockHashLegacy()) != phase4ActivationCache.end())
        return phase4ActivationCache[pIndex->GetBlockHashLegacy()];

    if (phase4ActivationHash == uint256())
        phase4ActivationHash = ppow2witdbview->GetPhase4ActivationHash();
    if (phase4ActivationHash != uint256())
    {
        const CBlockIndex* pIndexPrev = pIndex;
        while (pIndexPrev)
        {
            if (pIndexPrev->GetBlockHashPoW2() == phase4ActivationHash)
            {
                phase4ActivationCache[pIndex->GetBlockHashLegacy()] = true;
                return true;
            }
            pIndexPrev = pIndexPrev->pprev;
        }
    }

    //PoS version bits
    bool ret = (VersionBitsState(pIndex, params, Consensus::DEPLOYMENT_POW2_PHASE4, versionbitscache) == THRESHOLD_ACTIVE);
    if (ret)
    {
        ppow2witdbview->SetPhase4ActivationHash(phase3ActivationHash);
    }
    phase4ActivationCache[pIndex->GetBlockHashLegacy()] = ret;
    return ret;
}


// Phase 5 becomes active after all 'backwards compatible' witness addresses have been flushed from the system.
// At this point 'backwards compatible' witness addresses in the chain are treated simply as anyone can spend transactions (as they have all been spent this is fine) and old witness data is discarded.
std::map<uint256, bool> phase5ActivationCache;
bool IsPow2Phase5Active(const CBlockIndex* pIndex, const CChainParams& params)
{
    LOCK2(cs_main, pactiveWallet?&pactiveWallet->cs_wallet:NULL);

    if (!pIndex)
        return false;

    if (phase5ActivationCache.find(pIndex->GetBlockHashLegacy()) != phase5ActivationCache.end())
        return phase5ActivationCache[pIndex->GetBlockHashLegacy()];

    if (!IsPow2Phase4Active(pIndex, params.GetConsensus()))
    {
        phase5ActivationCache[pIndex->GetBlockHashLegacy()] = false;
        return false;
    }

    std::map<COutPoint, Coin> allWitnessCoins = getAllUnspentWitnessCoins(chainActive, params, pIndex);

    // If any PoW2WitnessOutput remain then we aren't active yet.
    for (auto iter : allWitnessCoins)
    {
        if (iter.second.out.GetType() != CTxOutType::PoW2WitnessOutput)
        {
            phase5ActivationCache[pIndex->GetBlockHashLegacy()] = false;
            return false;
        }
    }

    phase5ActivationCache[pIndex->GetBlockHashLegacy()] = true;
    return true;
}


bool IsPow2WitnessingActive(const CBlockIndex* pIndex, const CChainParams& chainparams)
{
    LOCK2(cs_main, pactiveWallet?&pactiveWallet->cs_wallet:NULL);

    if (!pIndex)
        return false;

    return IsPow2Phase3Active(pIndex, chainparams) || IsPow2Phase4Active(pIndex, chainparams) || IsPow2Phase5Active(pIndex, chainparams);
}

int GetPoW2Phase(const CBlockIndex* pIndex, const CChainParams& chainparams)
{
    LOCK2(cs_main, pactiveWallet?&pactiveWallet->cs_wallet:NULL);

    if (IsPow2Phase5Active(pIndex, chainparams))
    {
        return 5;
    }
    if (IsPow2Phase4Active(pIndex, chainparams))
    {
        return 4;
    }
    if (IsPow2Phase3Active(pIndex, chainparams))
    {
        return 3;
    }
    if (IsPow2Phase2Active(pIndex, chainparams))
    {
        return 2;
    }
    return 1;
}

bool IsPow2Phase2Active(const CBlockIndex* pindexPrev, const CChainParams& chainparams) { return IsPow2Phase2Active(pindexPrev, chainparams.GetConsensus()); }
bool IsPow2Phase4Active(const CBlockIndex* pindexPrev, const CChainParams& chainparams) { return IsPow2Phase4Active(pindexPrev, chainparams.GetConsensus()); }


//NB! nAmount is already in internal monetary format (8 zeros) form when entering this function - i.e. the nAmount for 22 NLG is '2200000000' and not '22'
int64_t GetPoW2RawWeightForAmount(int64_t nAmount, int64_t nLockLengthInBlocks)
{
    // We rebase the entire formula to to match internal monetary format (8 zeros), so that we can work with fixed point precision.
    // We rebase to 10 at the end for the final weight.
    arith_uint256 base = arith_uint256(COIN);
    #define BASE(x) (arith_uint256(x)*base)
    arith_uint256 Quantity = arith_uint256(nAmount);
    arith_uint256 BlocksPerYear = arith_uint256(365 * 576);
    arith_uint256 nWeight = ((BASE(Quantity)) + ((Quantity*Quantity) / arith_uint256(100000))) * (BASE(1) + (BASE(nLockLengthInBlocks) / BlocksPerYear));
    #undef BASE
    nWeight /= base;
    nWeight /= base;
    nWeight /= base;
    return nWeight.GetLow64();
}


int64_t GetPoW2LockLengthInBlocksFromOutput(CTxOut& out, uint64_t txBlockNumber)
{
    //fixme: (GULDEN) (2.0) - Check for off by 1 error (lockUntil - lockFrom)
    if ( (out.GetType() <= CTxOutType::ScriptOutput && out.output.scriptPubKey.IsPoW2Witness()) )
    {
        CTxOutPoW2Witness witnessDetails;
        out.output.scriptPubKey.ExtractPoW2WitnessFromScript(witnessDetails);
        return witnessDetails.lockUntilBlock - ( witnessDetails.lockFromBlock == 0 ? txBlockNumber : witnessDetails.lockFromBlock);
    }
    else if (out.GetType() == CTxOutType::PoW2WitnessOutput)
    {
        return out.output.witnessDetails.lockUntilBlock - ( out.output.witnessDetails.lockFromBlock == 0 ? txBlockNumber : out.output.witnessDetails.lockFromBlock);
    }
    return 0;
}

//fixme: Handle reorganisations that invalidate the cache.
int64_t GetPoW2Phase3ActivationTime()
{
    if (phase3ActivationHash == uint256())
        phase3ActivationHash = ppow2witdbview->GetPhase3ActivationHash();

    if (phase3ActivationHash != uint256())
    {
        if (mapBlockIndex.count(phase3ActivationHash))
        {
            CBlockIndex* pIndex = mapBlockIndex[phase3ActivationHash];
            return pIndex->nTime;
        }
    }
    else
    {
        if (IsPow2Phase3Active(chainActive.Tip(), Params()))
        {
            CBlockIndex* pIndex = chainActive.Tip();
            while (pIndex->pprev && IsPow2Phase3Active(pIndex->pprev, Params()))
            {
                pIndex = pIndex->pprev;
            }
            phase3ActivationHash = pIndex->GetBlockHashPoW2();
            ppow2witdbview->SetPhase3ActivationHash(phase3ActivationHash);
            return pIndex->nTime;
        }
    }
    return std::numeric_limits<int64_t>::max();
}

void GetPow2NetworkWeight(const CBlockIndex* pIndex, const CChainParams& chainparams, int64_t& nNumWitnessAddresses, int64_t& nTotalWeight)
{
    LOCK2(cs_main, pactiveWallet?&pactiveWallet->cs_wallet:NULL);

    std::map<COutPoint, Coin> allWitnessCoins = getAllUnspentWitnessCoins(chainActive, chainparams, pIndex);

    nNumWitnessAddresses = 0;
    nTotalWeight = 0;

    for (auto iter : allWitnessCoins)
    {
        if (pIndex == nullptr || iter.second.nHeight <= pIndex->nHeight)
        {
            CTxOut output = iter.second.out;

            nTotalWeight += GetPoW2RawWeightForAmount(output.nValue, GetPoW2LockLengthInBlocksFromOutput(output, iter.second.nHeight));
            ++nNumWitnessAddresses;
        }
    }
}



CBlockIndex* GetPoWBlockForPoSBlock(const CBlockIndex* pIndex)
{
    uint256 powHash = pIndex->GetBlockHashLegacy();
    if (!mapBlockIndex.count(powHash))
        return NULL;
    return mapBlockIndex[powHash];
}




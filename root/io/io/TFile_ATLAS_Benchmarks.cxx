#include "TFile.h"
#include "TTree.h"
#include "TSystem.h"

#include "benchmark/benchmark.h"
#include "rootbench/RBConfig.h"

#include <map>

static std::string GetAlgoName(int algo) {
    std::map<int, std::string> algoName = {
        {1, "zlib"},
        {2, "lzma"},
        {4, "lz4"},
        {5, "zstd"},
        {6, "lz4bs"}
    };

    if (algoName.find(algo) != algoName.end())
        return algoName[algo];
    else
        return "error";
}

static void BM_ATLAS_Decompress(benchmark::State &state, int algo) {

    int comp_level = state.range(0);
    std::string comp_setting = std::to_string(algo * 100 + comp_level);
    std::string old_filename = (RB::GetDataDir() + "/gg_data-zstd.root").c_str();
    std::string new_filename = "level_" + std::to_string(comp_level) + "_atlas_" + GetAlgoName(algo) + ".root";

    gSystem->Exec(("hadd -v 0 -f" + comp_setting + " " + new_filename + " " + old_filename).c_str());

    TFile *newfile = new TFile(new_filename.c_str());
    state.counters["comp_size"] = newfile->GetSize();
    newfile->Close();

    for (auto _ : state) {

        TFile *hfile = new TFile(new_filename.c_str());
        TTree *tree = (TTree*)hfile->Get("mini");

        Int_t nevent = (Int_t)tree->GetEntries();

        Int_t nb = 0;
        Int_t ev;

        for (ev = 0; ev < nevent; ev++) {
            nb += tree->GetEntry(ev);
        }

        hfile->Close();

    }

    gSystem->Exec(("rm -f " + new_filename).c_str());
}


static void BM_ATLAS_Decompress_ZLIB(benchmark::State &state) {
    BM_ATLAS_Decompress(state, 1);
}
static void BM_ATLAS_Decompress_LZMA(benchmark::State &state) {
    BM_ATLAS_Decompress(state, 2);
}
static void BM_ATLAS_Decompress_LZ4(benchmark::State &state) {
    BM_ATLAS_Decompress(state, 4);
}
static void BM_ATLAS_Decompress_ZSTD(benchmark::State &state) {
    BM_ATLAS_Decompress(state, 5);
}
static void BM_ATLAS_Decompress_LZ4BS(benchmark::State &state) {
    BM_ATLAS_Decompress(state, 6);
}


BENCHMARK(BM_ATLAS_Decompress_ZLIB)
->Arg(1)->Arg(6)->Arg(9)
->Unit(benchmark::kMillisecond)->Iterations(5);

BENCHMARK(BM_ATLAS_Decompress_LZMA)
->Arg(1)->Arg(6)->Arg(9)
->Unit(benchmark::kMillisecond)->Iterations(5);

BENCHMARK(BM_ATLAS_Decompress_LZ4)
->Arg(1)->Arg(6)->Arg(9)
->Unit(benchmark::kMillisecond)->Iterations(5);

BENCHMARK(BM_ATLAS_Decompress_ZSTD)
->Arg(1)->Arg(6)->Arg(9)
->Unit(benchmark::kMillisecond)->Iterations(5);

BENCHMARK(BM_ATLAS_Decompress_LZ4BS)
->Arg(1)->Arg(6)->Arg(9)
->Unit(benchmark::kMillisecond)->Iterations(5);


BENCHMARK_MAIN();
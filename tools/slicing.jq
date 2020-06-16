{
  slices: .eNB_config?[0].eNB.cellConfig[0]?.sliceConfig | {
    algorithm: .algorithm,
    dl: .dl
  },
  UEs: [
    .eNB_config?[0].UE.ueConfig[]? | {
      rnti: .rnti,
      imsi: .imsi,
      dlSliceId: .dlSliceId,
    }
  ]
}

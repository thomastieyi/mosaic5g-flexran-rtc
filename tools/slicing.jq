{
  slices: .eNB_config?[0].eNB.cellConfig[0]?.sliceConfig | {
    algorithm: .algorithm,
    dl: .dl
  }
}

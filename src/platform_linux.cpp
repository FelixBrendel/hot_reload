// TODO(Felix): Write the linux platform layer:
//   Requirements are:
//     - Create a `Game_Memory' instance
//     - Create a `Platform_Functions' instance
//     - load the game lib and it's `tick' function
//     - in a loop call the said `tick' function
//       - passing the game memory and platform functions
//     - in each loop iteration check if the lib changed since the last time
//     - if it did, reload it

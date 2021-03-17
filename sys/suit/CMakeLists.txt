add_riot_module(suit
  conditions.c
  handlers.c
  handlers_command_seq.c
  handlers_common.c
  handlers_envelope.c
  handlers_global.c
  policy.c
  suit.c
)
riot_module_depends(suit nanocbor libcose uuid)

add_riot_module(suit_transport_mock transport/mock.c)
riot_module_depends(suit_transport_mock suit riotboot riotboot_slot riotboot_flashwrite riotboot_flashwrite_verify_sha256)

add_riot_module(suit_transport_coap transport/coap.c)
riot_module_depends(suit_transport_coap suit nanocoap)

add_riot_module(suit_storage storage/storage.c)
riot_module_depends(suit_storage suit)

add_riot_module(suit_storage_ram storage/ram.c)
riot_module_depends(suit_storage_ram suit_storage)

add_riot_module(suit_storage_flashwrite storage/flashwrite.c)
riot_module_depends(suit_storage_flashwrite suit_storage)

#include "qspi_flash.h"

#include <stdint.h>
#include <string.h>
#include "web_index.h"

/*
 * The web interface is stored in the external N25Q128A QuadSPI flash.
 * During initialization, the firmware verifies the flash device and checks
 * the asset header. If the asset is valid, the flash is configured in
 * memory-mapped mode and the HTML content becomes accessible at 0x90000000.
 *
 * The web asset must be programmed into the external flash separately before
 * running the application. If it is unavailable or invalid, the web server
 * falls back to the embedded HTML resource.
 */

#define QSPI_BASE_ADDR            0x90000000u
#define QSPI_ASSET_OFFSET         0x00000000u
#define QSPI_ASSET_HEADER_SIZE    16u
#define QSPI_PAGE_SIZE            256u
#define QSPI_SECTOR_SIZE          4096u
#define QSPI_TIMEOUT_MS           5000u

#define N25Q_CMD_READ_ID          0x9Fu
#define N25Q_CMD_READ_STATUS      0x05u
#define N25Q_CMD_WRITE_ENABLE     0x06u
#define N25Q_CMD_SUBSECTOR_ERASE  0x20u
#define N25Q_CMD_PAGE_PROGRAM     0x02u
#define N25Q_CMD_FAST_READ        0x0Bu

#define N25Q_STATUS_WIP           0x01u
#define N25Q_STATUS_WEL           0x02u
#define N25Q_MANUFACTURER_MICRON  0x20u

static const uint8_t asset_magic[8] = {
  'A', 'D', 'X', 'L', 'W', 'E', 'B', '1'
};

static QSPI_HandleTypeDef *qspi_handle;
static bool qspi_ready;

static void command_defaults(QSPI_CommandTypeDef *cmd)
{
  memset(cmd, 0, sizeof(*cmd));
  cmd->InstructionMode = QSPI_INSTRUCTION_1_LINE;
  cmd->AddressMode = QSPI_ADDRESS_NONE;
  cmd->AlternateByteMode = QSPI_ALTERNATE_BYTES_NONE;
  cmd->DataMode = QSPI_DATA_NONE;
  cmd->DummyCycles = 0;
  cmd->DdrMode = QSPI_DDR_MODE_DISABLE;
  cmd->DdrHoldHalfCycle = QSPI_DDR_HHC_ANALOG_DELAY;
  cmd->SIOOMode = QSPI_SIOO_INST_EVERY_CMD;
}

static bool qspi_read_id(uint8_t id[3])
{
  QSPI_CommandTypeDef cmd;

  command_defaults(&cmd);
  cmd.Instruction = N25Q_CMD_READ_ID;
  cmd.DataMode = QSPI_DATA_1_LINE;
  cmd.NbData = 3;

  return HAL_QSPI_Command(qspi_handle, &cmd, QSPI_TIMEOUT_MS) == HAL_OK &&
         HAL_QSPI_Receive(qspi_handle, id, QSPI_TIMEOUT_MS) == HAL_OK;
}

static bool qspi_read(uint32_t address, uint8_t *data, uint32_t length)
{
  QSPI_CommandTypeDef cmd;

  command_defaults(&cmd);
  cmd.Instruction = N25Q_CMD_FAST_READ;
  cmd.AddressMode = QSPI_ADDRESS_1_LINE;
  cmd.AddressSize = QSPI_ADDRESS_24_BITS;
  cmd.Address = address;
  cmd.DataMode = QSPI_DATA_1_LINE;
  cmd.NbData = length;
  cmd.DummyCycles = 8;

  return HAL_QSPI_Command(qspi_handle, &cmd, QSPI_TIMEOUT_MS) == HAL_OK &&
         HAL_QSPI_Receive(qspi_handle, data, QSPI_TIMEOUT_MS) == HAL_OK;
}

static bool qspi_asset_valid(void)
{
  uint8_t header[QSPI_ASSET_HEADER_SIZE];
  uint32_t stored_len;
  uint32_t expected_len = (uint32_t)strlen(web_index_html) + 1U;

  if (!qspi_read(QSPI_ASSET_OFFSET, header, sizeof(header))) {
    return false;
  }

  stored_len = (uint32_t)header[8] |
               ((uint32_t)header[9] << 8) |
               ((uint32_t)header[10] << 16) |
               ((uint32_t)header[11] << 24);

  return memcmp(header, asset_magic, sizeof(asset_magic)) == 0 &&
         stored_len == expected_len;
}

static bool qspi_memory_mapped(void)
{
  QSPI_CommandTypeDef cmd;
  QSPI_MemoryMappedTypeDef cfg;

  command_defaults(&cmd);
  cmd.Instruction = N25Q_CMD_FAST_READ;
  cmd.AddressMode = QSPI_ADDRESS_1_LINE;
  cmd.AddressSize = QSPI_ADDRESS_24_BITS;
  cmd.DataMode = QSPI_DATA_1_LINE;
  cmd.DummyCycles = 8;

  memset(&cfg, 0, sizeof(cfg));
  cfg.TimeOutActivation = QSPI_TIMEOUT_COUNTER_DISABLE;

  return HAL_QSPI_MemoryMapped(qspi_handle, &cmd, &cfg) == HAL_OK;
}

bool app_qspi_init(QSPI_HandleTypeDef *hqspi)
{
  uint8_t id[3];

  qspi_ready = false;
  qspi_handle = hqspi;

  if (qspi_handle == NULL || !qspi_read_id(id) ||
      id[0] != N25Q_MANUFACTURER_MICRON) {
    return false;
  }

  /* The HTML asset is programmed directly into the external QuadSPI flash with
   * the debugger (see scripts/qspi.cfg), not written here at boot. We only
   * verify the signature is present, then enable memory-mapped reads. */
  if (!qspi_asset_valid()) {
    return false;
  }

  qspi_ready = qspi_memory_mapped();
  return qspi_ready;
}

const char *app_qspi_web_index(void)
{
  return (const char *)(QSPI_BASE_ADDR + QSPI_ASSET_OFFSET + QSPI_ASSET_HEADER_SIZE);
}

bool app_qspi_web_index_ready(void)
{
  return qspi_ready;
}

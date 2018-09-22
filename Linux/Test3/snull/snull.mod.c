#include <linux/module.h>
#include <linux/vermagic.h>
#include <linux/compiler.h>

MODULE_INFO(vermagic, VERMAGIC_STRING);

struct module __this_module
__attribute__((section(".gnu.linkonce.this_module"))) = {
	.name = KBUILD_MODNAME,
	.init = init_module,
#ifdef CONFIG_MODULE_UNLOAD
	.exit = cleanup_module,
#endif
	.arch = MODULE_ARCH_INIT,
};

static const struct modversion_info ____versions[]
__used
__attribute__((section("__versions"))) = {
	{ 0x35ec255d, "module_layout" },
	{ 0x15692c87, "param_ops_int" },
	{ 0x123c9dba, "register_netdev" },
	{ 0x714de7e3, "alloc_netdev_mqs" },
	{ 0x861a1132, "free_netdev" },
	{ 0x54b503bb, "unregister_netdev" },
	{ 0xfc3e7a17, "netif_rx" },
	{ 0x8a0f1925, "napi_complete" },
	{ 0x6128b5fc, "__printk_ratelimit" },
	{ 0x5d251494, "__netdev_alloc_skb" },
	{ 0xa092d300, "netif_receive_skb" },
	{ 0x63c1860a, "eth_type_trans" },
	{ 0xc5a02b84, "skb_put" },
	{ 0xf0fdf6cb, "__stack_chk_fail" },
	{ 0x7d11c268, "jiffies" },
	{ 0x37a0cba, "kfree" },
	{ 0x8cc6145f, "netif_napi_add" },
	{ 0x9b2f7169, "ether_setup" },
	{ 0xcea0a119, "kmalloc_caches" },
	{ 0x41ad0272, "kmem_cache_alloc_trace" },
	{ 0x16305289, "warn_slowpath_null" },
	{ 0xcb783cca, "__netif_schedule" },
	{ 0x50eedeb8, "printk" },
	{ 0x39f29937, "skb_push" },
	{ 0xf97456ea, "_raw_spin_unlock_irqrestore" },
	{ 0x21fb443e, "_raw_spin_lock_irqsave" },
	{ 0x2e60bace, "memcpy" },
	{ 0xfd33629, "consume_skb" },
	{ 0x9a67b9fb, "__napi_schedule" },
	{ 0x76ebea8, "pv_lock_ops" },
	{ 0x67f7403e, "_raw_spin_lock" },
	{ 0xb4390f9a, "mcount" },
};

static const char __module_depends[]
__used
__attribute__((section(".modinfo"))) =
"depends=";


MODULE_INFO(srcversion, "8BE4867D0F622AAC50D8A8D");

#!/usr/bin/env python3
# Copyright (c) 2020-2021 The DigiByte Core developers
# Distributed under the MIT software license, see the accompanying
# file COPYING or http://www.opensource.org/licenses/mit-license.php.
"""Test orphaned block rewards in the wallet."""

from test_framework.test_framework import DigiByteTestFramework
from test_framework.util import assert_equal
from test_framework.blocktools import COINBASE_MATURITY

class OrphanedBlockRewardTest(DigiByteTestFramework):
    def set_test_params(self):
        self.setup_clean_chain = True
        self.num_nodes = 2

    def skip_test_if_missing_module(self):
        self.skip_if_no_wallet()

    def run_test(self):
        # Generate some blocks and obtain some coins on node 0.  We send
        # some balance to node 1, which will hold it as a single coin.
        self.generate(self.nodes[0], COINBASE_MATURITY + 50)
        self.nodes[0].sendtoaddress(self.nodes[1].getnewaddress(), 10)
        self.generate(self.nodes[0], 1)

        # Get a block reward with node 1 and remember the block so we can orphan
        # it later.
        self.sync_blocks()
        blk = self.generate(self.nodes[1], 1)[0]

        # Let the block reward mature and send coins including both
        # the existing balance and the block reward.
        self.generate(self.nodes[0], COINBASE_MATURITY + 50)
        assert_equal(self.nodes[1].getbalance(), 10 + 72000)
        txid = self.nodes[1].sendtoaddress(self.nodes[0].getnewaddress(), 30)

        # Orphan the block reward and make sure that the original coins
        # from the wallet can still be spent.
        self.nodes[0].invalidateblock(blk)
        self.generate(self.nodes[0], COINBASE_MATURITY + 52)

        assert_equal(self.nodes[1].getbalances()["mine"], {
          "trusted": 10,
          "untrusted_pending": 0,
          "immature": 0,
        })
        self.nodes[1].sendtoaddress(self.nodes[0].getnewaddress(), 9)

if __name__ == '__main__':
    OrphanedBlockRewardTest().main()

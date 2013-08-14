"""
Graph for reasons of pgraph solution 
"""

__author__ = "Vita Loginova"
__date__ = "2013-06-28"

from  _compat import *
import Queue
from mybuild.pgraph import Reason
import heapq
import solver

import util, logging
logger = util.get_extended_logger(__name__)

class NodeContainer(object):
    
    @property
    def length(self):
        if self.members:
            return sum(r.length for r in self.members)
        else:
            return self._length

    @length.setter
    def length(self, value):
        self._length = value
    
    def __init__(self, literals):
        self.literals = frozenset(literals) 
        self.containers = set() #NodeContainrs set that contains nodes with
                                #current node as member 
        self.members = set()    #Node set of nodes with one literal from 
                                #node.literals
        self.therefore = {} #key = node, value = reason
        self.becauseof = {} #key = node, value = reason
        self._length = float("+inf")
        self.parent = None
        
    def __lt__(self, other):
        return self.length < other.length
    
    def compare_literals(self, literals):
        return (set(literals) == self.literals)
    
    def __repr__(self):
        return ("<{cls.__name__}: {literals}>"
                .format(cls=type(self), literals=list(self.literals)))
    
# TODO remove to other place after all types why function realization
def why_violation(literal, *cause_literals):
    return '%s because of violation %s' % (literal, cause_literals)  

class Rgraph(object): 
    """
    Rgraph or Reason graph
    """  
    def __init__(self, trunk, use_dead_branches = True):
        self.trunk = trunk
        self.initial = NodeContainer(set())  
        self.nodes = {}    
        self.nodes[frozenset(set())] = self.initial
        self.violation_graphs = {}
        
        solver.expand_branchset(trunk, ignore_errors=True)
        
        logger.dump(trunk)
        
        for node in trunk.nodes:
            self.nodes[frozenset([node[True]])] = NodeContainer(frozenset([node[True]]))
            self.nodes[frozenset([node[False]])] = NodeContainer(frozenset([node[False]]))
                
        for reason in trunk.reasons:
            self.fill_data(reason)
        
        if use_dead_branches:      
            for literal, branch in iteritems(trunk.dead_branches):
                self.violation_graphs[literal] = Rgraph(trunk, False)
                self.violation_graphs[literal].add_branch(branch)
                for gen_literal in branch.gen_literals:
                    self.fill_data(Reason(why_violation, ~gen_literal, gen_literal))
                    self.fill_data(Reason(None, gen_literal))
                                 
    def add_branch(self, branch):
        if self.trunk is not branch.trunk:
            raise ValueError('Branch must belong to the rgraph trunk')
         
        for literal in branch.gen_literals:
            self.fill_data(Reason(None, literal))       
        for reason in branch.reasons:
            self.fill_data(reason) 
         
    def fill_data(self, reason): 
        if len(reason.cause_literals) > 1:
            s = frozenset(reason.cause_literals)
            if s not in self.nodes:
                self.nodes[s] = NodeContainer(s)
                self.update_containers(self.nodes[s])
                     
        cause_node = self.nodes[frozenset(reason.cause_literals)]
        literal_node = self.nodes[frozenset([reason.literal])]
        
        cause_node.therefore[literal_node] = reason
        literal_node.becauseof[cause_node] = reason
        
    def update_containers(self, node):
        for literal in node.literals:
            member = self.nodes[frozenset([literal])]
            node.members.add(member)
            member.containers.add(node)  

    def find_shortest_ways(self):
        """
        This algorithm a common Dijkstra's algorithm with small modification,
        length of node of more one reason is computed as sum of it's becauseof.
        After function applying each node contains field length, the length of 
        the shortest way to the initial nodes. Parent is the previous node in 
        the shortest way.
        """ 
        
        for rgraph in self.violation_graphs.values():
            rgraph.find_shortest_ways()

        queue = []  
        used = set()
        
        def push(node):
            heapq.heappush(queue, (node.length, node))
            
        def pop():
            node = heapq.heappop(queue)[1]
            if node in used:
                return pop()       
            used.add(node)
            return node 
                    
        for node in self.initial.therefore:
            node.length = 0
            node.parent = node 
            push(node)
         
        heapq.heapify(queue)     
            
        while queue:
            try:
                node = pop()
            except IndexError:
                continue        
                     
            for cons in node.therefore:
                if cons.length > node.length + 1:
                    cons.length = node.length + 1
                    cons.parent = node
                    push(cons)
                    for container in cons.containers:
                        push(container)
                        
    def print_graph(self):
        """
        Simple way to print reason graph. Nodes of more one reason are printed 
        in new line without offset.
        """
        queue = Queue.LifoQueue()
        used = set()
        
        #queue contains touples (node, reason)
        for node in self.initial.therefore:
            queue.put((node, self.initial.therefore[node]))
            self._process_containers_dfs(node, used, queue)
        
        while not queue.empty():
            node, reason = queue.get()        
            self.dfs(node, reason, used, queue, 0)
            
        for literal, branch in iteritems(self.trunk.dead_branches): 
            violation_nodes = self.get_violation_nodes(branch)
            
            print '--- violation for', branch.gen_literals, '---'
            
            rgraph_branch = self.violation_graphs[literal]
                            
            for node in violation_nodes:
                print '------ because of {0} and {1} ------'.format(node[True], node[False])
                rgraph_branch.print_shortest_way(node[True])
                rgraph_branch.print_shortest_way(node[False])
                print '------'
            print '---'
            
    def get_violation_nodes(self, branch):
        violation_nodes = set()
        literals = self.trunk.literals | branch.literals
        nodes = self.trunk.nodes | branch.nodes
        for node in nodes:
            if node[False] in literals and node[True] in literals:
                violation_nodes.add(node)
        return violation_nodes
                
    def dfs(self, node, reason, used, queue, depth):
        if node in used:
            self.print_reason(reason,depth)
            return
        
        used.add(node)
        self.print_reason(reason,depth)
        for cons in node.therefore:
            self.dfs(cons, node.therefore[cons], used, queue, depth + 1)
            self._process_containers_dfs(cons, used, queue)
                            
    def _process_containers_dfs(self, node, used, queue):
        for container in node.containers:
            if container not in used:
                used.add(container)
                for ccons in container.therefore:
                    if node not in queue.queue:
                        queue.put((ccons, container.therefore[ccons]))
                            
    def print_reason(self, reason, depth):
        print '  ' * depth, reason

    def print_shortest_way(self, literals):
        node = self.nodes[frozenset([literals])]    
        if node.length == float("+inf"):
            raise Exception   
        self._print_shortest_way_part(node)
         
    def _print_shortest_way_part(self, node):
        if node.length == 0:
            if not node.members:
                self.print_reason(node.becauseof[self.initial], 0)
            else:
                for member in node.members:
                    self._print_shortest_way_part(member)
            return 1;
               
        if not node.members:
            depth = self._print_shortest_way_part(node.parent)
            self.print_reason(node.becauseof[node.parent], depth)
            return depth + 1
        else:
            for member in node.members:
                self._print_shortest_way_part(member)
            return 0

